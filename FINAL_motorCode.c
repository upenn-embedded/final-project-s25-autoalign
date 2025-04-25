#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

#define PAYLOAD_SIZE 2

// --- Stepper Motor (PB2?PB5) ---
#define STEPPER_PORT   PORTB
#define STEPPER_DDR    DDRB
#define STEPPER_MASK   ((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5))
#define STEP_SHIFT     PB2

// X-axis motor (Left/Right)
#define X_STEP PD1
#define X_DIR  PD5

// Y-axis motor (Up/Down)
#define Y_STEP PD2
#define Y_DIR  PD3

const uint8_t fullStep[4] = {
    0b0011,
    0b0110,
    0b1100,
    0b1001
};

static uint8_t idx = 0;

void forward(uint16_t steps) {
    for (uint16_t i = 0; i < steps; i++) {
        STEPPER_PORT = (STEPPER_PORT & ~STEPPER_MASK)
                     | ((fullStep[idx] << STEP_SHIFT) & STEPPER_MASK);
        _delay_ms(2);
        idx = (idx + 1) & 0x03;
    }
}

void backward(uint16_t steps) {
    for (uint16_t i = 0; i < steps; i++) {
        STEPPER_PORT = (STEPPER_PORT & ~STEPPER_MASK)
                     | ((fullStep[idx] << STEP_SHIFT) & STEPPER_MASK);
        _delay_ms(2);
        idx = (idx + 3) & 0x03;  // reverse index (mod 4)
    }
}

// --- nRF24L01+ (SPI1) ---
#define CSN_DDR   DDRC
#define CSN_PORT  PORTC
#define CSN_PIN   PC5
#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CE_PIN    PB0
#define IRQ_DDR   DDRB
#define IRQ_PORT  PORTB
#define IRQ_PIN   PB1

static inline void CSN_LOW()   { CSN_PORT &= ~(1<<CSN_PIN); }
static inline void CSN_HIGH()  { CSN_PORT |=  (1<<CSN_PIN); }
static inline void CE_HIGH()   { CE_PORT  |=  (1<<CE_PIN); }
static inline void CE_LOW()    { CE_PORT  &= ~(1<<CE_PIN); }

#define R_REG     0x00
#define W_REG     0x20
#define R_RX_PAY  0x61
#define NOP       0xFF
#define CONFIG    0x00
#define EN_AA     0x01
#define EN_RXA    0x02
#define AW        0x03
#define CH        0x05
#define RF_SETUP  0x06
#define STATUS    0x07
#define RX_ADDR   0x0A
#define RX_PW     0x11
#define RX_DR     (1<<6)

#define PRIM_RX   (1<<0)
#define PWR_UP    (1<<1)
#define EN_CRC    (1<<3)
#define RX_DR     (1<<6)

uint8_t rx_addr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t buf[PAYLOAD_SIZE];

static uint8_t spi1_xfer(uint8_t d){
    SPDR1 = d;
    while (!(SPSR1 & (1<<SPIF1)));
    return SPDR1;
}

static void spi1_init(void) {
    PRR1 &= ~(1<<PRSPI1);
    DDRE |= (1<<PE3)|(1<<PE2);  // MOSI1, SCK1
    DDRC |= (1<<PC1);           // SS1
    DDRC &= ~(1<<PC0);          // MISO1
    SPCR1 = (1<<SPE1)|(1<<MSTR1)|(1<<SPR10);
    SPSR1 &= ~(1<<SPI2X1);
    CSN_LOW(); spi1_xfer(0x55); CSN_HIGH();
}

static void nrf_write(uint8_t reg, uint8_t val) {
    CSN_LOW();
    spi1_xfer(W_REG | reg);
    spi1_xfer(val);
    CSN_HIGH();
}

static uint8_t nrf_read(uint8_t reg) {
    uint8_t v;
    CSN_LOW();
    spi1_xfer(R_REG | reg);
    v = spi1_xfer(NOP);
    CSN_HIGH();
    return v;
}

static void nrf_read_payload(void) {
    CSN_LOW();
    spi1_xfer(R_RX_PAY);
    for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {
        buf[i] = spi1_xfer(NOP);
    }
    CSN_HIGH();
    nrf_write(STATUS, RX_DR); // clear RX_DR flag
}

void shoot(){
    backward(800);
    forward(800);
}

void setup_pins() {
    // Set all STEP and DIR pins as outputs
    DDRD |= (1 << X_STEP) | (1 << X_DIR) | (1 << Y_STEP) | (1 << Y_DIR);
}

void step_once(uint8_t step_pin) {
    PORTD |= (1 << step_pin);    // STEP high
    _delay_us(300);              // pulse width
    PORTD &= ~(1 << step_pin);   // STEP low
    _delay_us(300);
}

void step_quick(uint8_t step_pin) {
    PORTD |= (1 << step_pin);    // STEP high
    _delay_us(100);               // pulse width
    PORTD &= ~(1 << step_pin);   // STEP low
    _delay_us(100);
}

void move_motor(uint8_t dir_pin, uint8_t step_pin, uint8_t dir, uint16_t steps) {
    if (dir)
        PORTD |= (1 << dir_pin);     // DIR = 1
    else
        PORTD &= ~(1 << dir_pin);    // DIR = 0

     _delay_us(5); 
    for (uint16_t i = 0; i < steps; i++) {
        step_once(step_pin);
    }
}

void move_motor2(uint8_t dir_pin, uint8_t step_pin, uint8_t dir, uint16_t steps) {
    if (dir)
        PORTD |= (1 << dir_pin);     // DIR = 1
    else
        PORTD &= ~(1 << dir_pin);    // DIR = 0
 _delay_us(5); 
    for (uint16_t i = 0; i < steps; i++) {
        step_quick(step_pin);
    }
}

void moveDownLot(){
    move_motor2(Y_DIR, Y_STEP, 0, 20000); 
}

void moveDownLittle(){
    move_motor2(Y_DIR, Y_STEP, 0, 11500); 
}

void moveUpLot(){
    move_motor2(Y_DIR, Y_STEP, 1, 20000);
}

void moveUpLittle(){
    move_motor2(Y_DIR, Y_STEP, 1, 11500);
}

void moveRightLot(){
    move_motor(X_DIR, X_STEP, 1, 700); 
}
void moveRightLittle(){
    move_motor(X_DIR, X_STEP, 1, 200); 
}
void moveLeftLot(){
    // Explicitly use the direction value that worked in your test code
    move_motor(X_DIR, X_STEP, 0, 700); 
}
void moveLeftLittle(){
    move_motor(X_DIR, X_STEP, 0, 200);
}


volatile uint8_t cmd_received = 0;
volatile char cmd_letter = 0;
volatile char cmd_number = 0;
volatile uint8_t seconds_since_last_cmd = 0;
volatile uint8_t timer_enabled = 0;

// --- IRQ Handler ---
ISR(PCINT0_vect) {
    if (!(PINB & (1 << IRQ_PIN))) {
        if (nrf_read(STATUS) & RX_DR) {
            nrf_read_payload();
            //printf("IRQ: Received \"%c%c\"\r\n", buf[0], buf[1]);
            
            cmd_letter = buf[0];
            cmd_number = buf[1];
            cmd_received = 1;
  
            timer_enabled = 0;
            seconds_since_last_cmd = 0;
            
            // Clear the received flag
           nrf_write(STATUS, RX_DR);
        }
    }
}




void timer1_init(void) {
    // Set CTC mode: WGM12 = 1
    TCCR1B |= (1 << WGM12);

    // Prescaler = 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);

    // Compare match every 1 second
    // Formula: OCR1A = (F_CPU / prescaler / freq) - 1
    // For 1Hz: OCR1A = 16000000 / 1024 / 1 - 1 = 15624
    OCR1A = 15624;

    // Enable Timer1 Compare Match A interrupt
    TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    if (timer_enabled) {
        seconds_since_last_cmd++;

        if (seconds_since_last_cmd >= 7) {
            shoot();
            timer_enabled = 1;             // stop counting
            seconds_since_last_cmd = 0;    // reset
        }
    }
}


// --- Main ---
int main(void) {
   //uart_init();
    spi1_init();
    setup_pins();

    // Control and motor pins
    CSN_DDR  |= (1<<CSN_PIN);
    CE_DDR   |= (1<<CE_PIN);
    IRQ_DDR  &= ~(1<<IRQ_PIN);
    IRQ_PORT |=  (1<<IRQ_PIN);  // enable pull-up on IRQ
    STEPPER_DDR |= STEPPER_MASK;

    CSN_HIGH();
    CE_LOW();
    _delay_ms(100);

    // NRF init
    nrf_write(CONFIG, PRIM_RX | PWR_UP | EN_CRC);
    _delay_ms(5);
    nrf_write(EN_AA,  0);
    nrf_write(EN_RXA, 1);
    nrf_write(AW,     3);
    nrf_write(CH,     2);
    nrf_write(RF_SETUP, (1<<3)|(3<<1));

    CSN_LOW();
    spi1_xfer(W_REG | RX_ADDR);
    for (uint8_t i = 0; i < 5; i++) spi1_xfer(rx_addr[i]);
    CSN_HIGH();

    nrf_write(RX_PW, PAYLOAD_SIZE);
    nrf_write(STATUS, RX_DR);
    CSN_LOW(); spi1_xfer(0xE1); CSN_HIGH(); // flush TX
    CSN_LOW(); spi1_xfer(0xE2); CSN_HIGH(); // flush RX

    // Enable interrupt on PB1 (PCINT1)
    PCICR  |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT1);
    
    timer1_init();
    sei();

    
    //DDRC &= ~(1<<DDC1); //bullseye

    
    
    
    CE_HIGH();  // begin listening
    
    
    shoot();
    seconds_since_last_cmd = 0;
    timer_enabled = 1;

    while (1) {
        //printf("timer: %d\n",  seconds_since_last_cmd);
        if(cmd_received){
         if (buf[0] == 'Y' && buf[1] == '2') {
             moveDownLot();
         } 
         else if (buf[0] == 'R' && buf[1] == '1') {
             moveDownLot();
             _delay_ms(500);
             moveRightLot();
         } 
         else if (buf[0] == 'Y' && buf[1] == '4') {
             moveUpLot();
         } 
         else if (buf[0] == 'R' && buf[1] == '7') {
             moveDownLot();
             _delay_ms(500);
             moveLeftLot();
         } 
         else if (buf[0] == 'Y' && buf[1] == '3') {
             moveRightLot();
         } 
         else if (buf[0] == 'Y' && buf[1] == '1') {
             moveLeftLot();
         } 
         else if (buf[0] == 'R' && buf[1] == '4') {
             moveUpLot();
             _delay_ms(500);
             moveRightLot();
         } 
         else if (buf[0] == 'R' && buf[1] == '6') {
             moveUpLot();
             _delay_ms(500);
             moveLeftLot();
         } 
         else if (buf[0] == 'W' && buf[1] == '3') {
             moveDownLittle();
             _delay_ms(500);
             moveRightLittle();
         } 
         else if (buf[0] == 'R' && buf[1] == '3') {
             moveDownLittle();
         } 
         else if (buf[0] == 'W' && buf[1] == '2') {
             moveDownLittle();
             _delay_ms(500);
             moveLeftLittle();
         } 
         else if (buf[0] == 'R' && buf[1] == '5') {
             moveRightLittle();
         } 
         else if (buf[0] == 'W' && buf[1] == '1') {
             moveUpLittle();
             _delay_ms(500);
             moveRightLittle();
         } 
         else if (buf[0] == 'R' && buf[1] == '8') {
             moveUpLittle();
         } 
         else if (buf[0] == 'W' && buf[1] == '4') {
             moveUpLittle();
             _delay_ms(500);
             moveLeftLittle();
         } 
         else if (buf[0] == 'R' && buf[1] == '2') {
             moveLeftLittle();
         }
         else if (buf[0] == 'B' && buf[1] == '0') {
            moveLeftLot();
            _delay_ms(200);
             moveRightLot();
             _delay_ms(200);
             moveLeftLot();
            _delay_ms(200);
             moveRightLot();
             _delay_ms(200);
             moveLeftLot();
            _delay_ms(200);
             moveRightLot();
             _delay_ms(200);
             moveLeftLot();
            _delay_ms(200);
             moveRightLot();
             _delay_ms(200);
         }
         
         cmd_received = 0;
         seconds_since_last_cmd = 0;
         timer_enabled = 1;
         shoot();
        }
        
        
    }
}
