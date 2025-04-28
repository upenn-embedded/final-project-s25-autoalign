/* main_receiver_spi1_min.c - Destynn Keuchel Minimized nRF24L01+ RX via SPI1 on ATmega328PB */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define PAYLOAD_SIZE 2

// Pins
#define CSN_PORT PORTD; #define CSN_DDR DDRD; #define CSN_PIN PD5;
#define CE_PORT PORTD;  #define CE_DDR DDRD;  #define CE_PIN PD6;
static inline void CSN_LOW()  { PORTD &= ~(1<<PD5); }
static inline void CSN_HIGH() { PORTD |=  (1<<PD5); }
static inline void CE_HIGH()  { PORTD |=  (1<<PD6); }
static inline void CE_LOW()   { PORTD &= ~(1<<PD6); }

// SPI1 transfer
static uint8_t spi1_xfer(uint8_t d){ SPDR1=d; while(!(SPSR1&(1<<SPIF1))); return SPDR1; }

// nRF commands
#define R_REG    0x00
#define W_REG    0x20
#define R_RX_PAY 0x61
#define NOP      0xFF

// Registers
#define CONFIG   0x00
#define EN_AA    0x01
#define EN_RXA   0x02
#define AW       0x03
#define CH       0x05
#define RF_SETUP 0x06
#define STATUS   0x07
#define RX_ADDR  0x0A
#define RX_PW    0x11
#define FIFO_ST  0x17

#define PRIM_RX  (1<<0)
#define PWR_UP   (1<<1)
#define EN_CRC   (1<<3)
#define RX_DR    (1<<6)





// TB6612 PORTB pin assignments
#define AIN1x PB0
#define AIN2x PB1
#define PWMAx PB2
#define BIN1x PB3
#define BIN2x PB4
#define PWMBx PB5

#define AIN1y PD0
#define AIN2y PD1
#define PWMAy PD2
#define BIN1y PD3
#define BIN2y PD4
#define PWMBy PD7

uint8_t rx_addr[5]={0xE7,0xE7,0xE7,0xE7,0xE7}; // Changed to 5 bytes
uint8_t buf[PAYLOAD_SIZE];

static void spi1_init(void){ PRR1&=~(1<<PRSPI1);
    DDRE|=(1<<PE3)|(1<<PE2); DDRC|=(1<<PC1); DDRC&=~(1<<PC0);
    SPCR1=(1<<SPE1)|(1<<MSTR1)|(1<<SPR10); SPSR1&=~(1<<SPI2X1);
    CSN_LOW(); spi1_xfer(0x55); CSN_HIGH(); }

static void nrf_write(uint8_t r,uint8_t v){ CSN_LOW(); spi1_xfer(W_REG|r); spi1_xfer(v); CSN_HIGH(); }
static uint8_t nrf_read(uint8_t r){ uint8_t v; CSN_LOW(); spi1_xfer(R_REG|r); v=spi1_xfer(NOP); CSN_HIGH(); return v; }
static void nrf_read_payload(void){ 
    CSN_LOW(); 
    spi1_xfer(R_RX_PAY); 
    for(int i=0; i<PAYLOAD_SIZE; i++) {
        buf[i]=spi1_xfer(NOP); 
    }
    CSN_HIGH(); 
    nrf_write(STATUS,RX_DR);
} 


// Bitmasks for setting control lines (AIN1..BIN2)
const uint8_t halfstep_sequence_y[8] = {
    (1 << AIN1y) | (1 << BIN1y),               // Step 0: A+, B+
    (1 << BIN1y),                              // Step 1: B+
    (1 << AIN2y) | (1 << BIN1y),               // Step 2: A-, B+
    (1 << AIN2y),                              // Step 3: A-
    (1 << AIN2y) | (1 << BIN2y),               // Step 4: A-, B-
    (1 << BIN2y),                              // Step 5: B-
    (1 << AIN1y) | (1 << BIN2y),               // Step 6: A+, B-
    (1 << AIN1y),                              // Step 7: A+
};

const uint8_t halfstep_sequence_x[8] = {
    (1 << AIN1x) | (1 << BIN1x),               // Step 0: A+, B+
    (1 << BIN1x),                              // Step 1: B+
    (1 << AIN2x) | (1 << BIN1x),               // Step 2: A-, B+
    (1 << AIN2x),                              // Step 3: A-
    (1 << AIN2x) | (1 << BIN2x),               // Step 4: A-, B-
    (1 << BIN2x),                              // Step 5: B-
    (1 << AIN1x) | (1 << BIN2x),               // Step 6: A+, B-
    (1 << AIN1x),                              // Step 7: A+
};

void init_pins_y() {
    DDRB |= (1 << AIN1y) | (1 << AIN2y) | (1 << PWMAy) |
            (1 << BIN1y) | (1 << BIN2y) | (1 << PWMBy);
}
void init_pins_x() {
    DDRD |= (1 << AIN1x) | (1 << AIN2x) | (1 << PWMAx) |
            (1 << BIN1x) | (1 << BIN2x) | (1 << PWMBx);
}

void step_motor_y(uint8_t step) {
    uint8_t bits_y = halfstep_sequence_y[step % 8];

    // Clear direction bits first (PB0?PB4)
    PORTB &= ~((1 << AIN1y) | (1 << AIN2y) | (1 << BIN1y) | (1 << BIN2y));
    PORTB |= bits_y;

    // Enable both H-bridges
    PORTB |= (1 << PWMAy) | (1 << PWMBy);
}

void step_motor_x(uint8_t step) {
    uint8_t bits_x = halfstep_sequence_x[step % 8];

    // Clear direction bits first (PB0?PB4)
    PORTD &= ~((1 << AIN1x) | (1 << AIN2x) | (1 << BIN1x) | (1 << BIN2x));
    PORTD |= bits_x;

    // Enable both H-bridges
    PORTD |= (1 << PWMAx) | (1 << PWMBx);

}



int main(void){ 
    uart_init(); 
    spi1_init();
    DDRD|=(1<<PD5)|(1<<PD6); 
    CSN_HIGH(); 
    CE_LOW(); 
    _delay_ms(100);
    nrf_write(CONFIG,PRIM_RX|PWR_UP|EN_CRC); 
    _delay_ms(5);
    nrf_write(EN_AA,0); 
    nrf_write(EN_RXA,1); 
    nrf_write(AW,3);  // Address width is still 3 (setting 3 means 5 bytes address width)
    nrf_write(CH,2); 
    nrf_write(RF_SETUP,(1<<3)|(3<<1));
    
    // Write receive address
    CSN_LOW(); 
    spi1_xfer(W_REG|RX_ADDR); 
    for(int i=0; i<5; i++) {
        spi1_xfer(rx_addr[i]); 
    }
    CSN_HIGH();
    
    nrf_write(RX_PW,PAYLOAD_SIZE);
    nrf_write(STATUS,RX_DR);
    CSN_LOW(); spi1_xfer(0xE1); CSN_HIGH(); // FLUSH_TX
    CSN_LOW(); spi1_xfer(0xE2); CSN_HIGH(); // FLUSH_RX
    CE_HIGH();
    
    char Letter = '0';
    char Number = '0';
    
    init_pins_y();
    init_pins_x();

    uint8_t stepy = 0;
    uint8_t stepx = 0;
    int x = 0;
    int y = 0;
    
    while(1){ 
        x = 0;
        y = 0;
        
        step_motor_x(stepx++);
                _delay_ms(100); 
        step_motor_y(stepy++);
                _delay_ms(100);
                
                
        step_motor_x(stepx--);
                _delay_ms(100);
        step_motor_y(stepy--);
                _delay_ms(100); 
        if(nrf_read(STATUS)&RX_DR){
            nrf_read_payload(); 
            printf("Received: \""); 
            for(int i=0; i<2; i++) {
                putchar(buf[i]); 
            }
            printf("\"\n"); 
            
            Letter = buf[0];
            Number = buf[1];
            
            if ((Letter == 'Y') && (Number == '1')) {
                printf("Y1!!!!!!\n");
                x = -1;
                y = 0;
            }
            if ((Letter == 'Y') && (Number == '2')) {
                printf("Y2!!!!!!\n");
                x = 0;
                y = -1;
            }
            if ((Letter == 'Y') && (Number == '3')) {
                printf("Y3!!!!!!\n");
                x = 1;
                y = 0;
            }
            if ((Letter == 'Y') && (Number == '4')) {
                printf("Y4!!!!!!\n");
                x = 0;
                y = 1;
            }

            if ((Letter == 'W') && (Number == '1')) {
                printf("W1!!!!!!\n");
                x = 1;
                y = 1;
            }
            if ((Letter == 'W') && (Number == '2')) {
                printf("W2!!!!!!\n");
                x = -1;
                y = -1;
            }
            if ((Letter == 'W') && (Number == '3')) {
                printf("W3!!!!!!\n");
                x = 1;
                y = -1;
            }
            if ((Letter == 'W') && (Number == '4')) {
                printf("W4!!!!!!\n");
                x = -1;
                y = 1;
            }

            if ((Letter == 'R') && (Number == '1')) {
                printf("R1!!!!!!\n");
                x = 1;
                y = -1;
            }
            if ((Letter == 'R') && (Number == '2')) {
                printf("R2!!!!!!\n");
                x = -1;
                y = 0;
            }
            if ((Letter == 'R') && (Number == '3')) {
                printf("R3!!!!!!\n");
                x = 0;
                y = -1;
            }
            if ((Letter == 'R') && (Number == '4')) {
                printf("R4!!!!!!\n");
                x = 1;
                y = 1;
            }
            if ((Letter == 'R') && (Number == '5')) {
                printf("R5!!!!!!\n");
                x = 1;
                y = 0;
            }
            if ((Letter == 'R') && (Number == '6')) {
                printf("R6!!!!!!\n");
                x = -1;
                y = 1;
            }
            if ((Letter == 'R') && (Number == '7')) {
                printf("R7!!!!!!\n");
                x = -1;
                y = -1;
            }
            if ((Letter == 'R') && (Number == '8')) {
                printf("R8!!!!!!\n");
                x = 0;
                y = 1;
            }
            if(x=1){
               step_motor_x(stepx++);
                _delay_ms(100); 
            }
            if(x=-1){
               step_motor_x(stepx--);
                _delay_ms(100); 
            }
            if(y=1){
               step_motor_y(stepy++);
                _delay_ms(100); 
            }
            if(y=-1){
               step_motor_y(stepy--);
                _delay_ms(100); 
            }

  
        } 
    } 
}
