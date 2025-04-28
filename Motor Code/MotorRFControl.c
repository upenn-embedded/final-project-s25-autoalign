#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

#define PAYLOAD_SIZE 2

// nRF24L01 Pins
#define CSN_LOW()  (PORTD &= ~(1<<PD5))
#define CSN_HIGH() (PORTD |=  (1<<PD5))
#define CE_LOW()   (PORTD &= ~(1<<PD6))
#define CE_HIGH()  (PORTD |=  (1<<PD6))

// SPI1 Transfer
static uint8_t spi1_xfer(uint8_t d){ SPDR1=d; while(!(SPSR1&(1<<SPIF1))); return SPDR1; }

// nRF24L01+ Commands and Registers
#define R_REG    0x00
#define W_REG    0x20
#define R_RX_PAY 0x61
#define NOP      0xFF
#define CONFIG   0x00
#define EN_AA    0x01
#define EN_RXA   0x02
#define AW       0x03
#define CH       0x05
#define RF_SETUP 0x06
#define STATUS   0x07
#define RX_ADDR  0x0A
#define RX_PW    0x11
#define RX_DR    (1<<6)

uint8_t rx_addr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t buf[PAYLOAD_SIZE];

// Stepper Motor Pins
#define X_STEP PD1
#define X_DIR  PD0
#define Y_STEP PD3
#define Y_DIR  PD2

// Microstepping Pins (only for X motor)
#define MS1 PD4
#define MS2 PD5
#define MS3 PD6

void spi1_init(void){
    PRR1 &= ~(1<<PRSPI1);
    DDRE |= (1<<PE3)|(1<<PE2);
    DDRC |= (1<<PC1);
    DDRC &= ~(1<<PC0);
    SPCR1 = (1<<SPE1)|(1<<MSTR1)|(1<<SPR10);
    SPSR1 &= ~(1<<SPI2X1);
    CSN_LOW(); spi1_xfer(0x55); CSN_HIGH();
}

void nrf_write(uint8_t r, uint8_t v){
    CSN_LOW(); spi1_xfer(W_REG|r); spi1_xfer(v); CSN_HIGH();
}

uint8_t nrf_read(uint8_t r){
    uint8_t v; CSN_LOW(); spi1_xfer(R_REG|r); v = spi1_xfer(NOP); CSN_HIGH(); return v;
}

void nrf_read_payload(void){
    CSN_LOW(); spi1_xfer(R_RX_PAY); for (int i=0;i<PAYLOAD_SIZE;i++) buf[i] = spi1_xfer(NOP); CSN_HIGH();
    nrf_write(STATUS, RX_DR);
}

// --- Stepper Motor Movement ---
void step_motor(uint8_t step_pin){
    PORTD |= (1 << step_pin);
    _delay_us(200);
    PORTD &= ~(1 << step_pin);
    _delay_us(200);
}

void move_motor(uint8_t dir_pin, uint8_t step_pin, uint8_t dir, uint8_t steps){
    if (dir) PORTD |= (1 << dir_pin);
    else     PORTD &= ~(1 << dir_pin);
    for (uint8_t i = 0; i < steps; i++) {
        step_motor(step_pin);
    }
}

void move_left(uint8_t steps)  { move_motor(X_DIR, X_STEP, 0, steps); }
void move_right(uint8_t steps) { move_motor(X_DIR, X_STEP, 1, steps); }
void move_up(uint8_t steps)    { move_motor(Y_DIR, Y_STEP, 1, steps); }
void move_down(uint8_t steps)  { move_motor(Y_DIR, Y_STEP, 0, steps); }
void stop_all_motors()         { /* nothing needed for now */ }

// --- Zone Logic Handler ---
void handle_zone(char l, char n) {
    const uint8_t LOT = 13;
    const uint8_t LIT = 4;

    if (l == 'B' && n == '0') {
        stop_all_motors();
        printf("Bullseye hit!\n");
        while(1); // Stop further movement
    }

    if (l == 'R' && n == '1') { move_down(LOT); move_right(LOT); }
    else if (l == 'Y' && n == '2') { move_down(LOT); }
    else if (l == 'R' && n == '7') { move_down(LOT); move_left(LOT); }
    else if (l == 'Y' && n == '3') { move_right(LOT); }
    else if (l == 'Y' && n == '1') { move_left(LOT); }
    else if (l == 'R' && n == '4') { move_up(LOT); move_right(LOT); }
    else if (l == 'Y' && n == '4') { move_up(LOT); }
    else if (l == 'R' && n == '6') { move_up(LOT); move_left(LOT); }
    else if (l == 'W' && n == '3') { move_down(LIT); move_right(LIT); }
    else if (l == 'R' && n == '3') { move_down(LIT); }
    else if (l == 'W' && n == '2') { move_down(LIT); move_left(LIT); }
    else if (l == 'R' && n == '5') { move_right(LIT); }
    else if (l == 'W' && n == '1') { move_up(LIT); move_right(LIT); }
    else if (l == 'R' && n == '8') { move_up(LIT); }
    else if (l == 'W' && n == '4') { move_up(LIT); move_left(LIT); }
    else if (l == 'R' && n == '2') { move_left(LIT); }
}

// --- Main Function ---
int main(void){
    uart_init();
    spi1_init();

    // Set control pins as outputs
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6);
    
    // Microstepping for X motor (1/8 step)
    PORTD |= (1 << MS1);
    PORTD |= (1 << MS2);
    PORTD &= ~(1 << MS3);

    CSN_HIGH(); CE_LOW(); _delay_ms(100);

    nrf_write(CONFIG, (1<<1)|(1<<3)|(1<<0)); // PWR_UP | EN_CRC | PRIM_RX
    _delay_ms(5);
    nrf_write(EN_AA, 0); 
    nrf_write(EN_RXA, 1);
    nrf_write(AW, 3); 
    nrf_write(CH, 2);
    nrf_write(RF_SETUP, (1<<3)|(3<<1));

    CSN_LOW(); spi1_xfer(W_REG | RX_ADDR); for (int i = 0; i < 5; i++) spi1_xfer(rx_addr[i]); CSN_HIGH();
    nrf_write(RX_PW, PAYLOAD_SIZE);
    nrf_write(STATUS, RX_DR);

    CSN_LOW(); spi1_xfer(0xE2); CSN_HIGH(); // FLUSH_RX
    CE_HIGH();

    while (1) {
        if (nrf_read(STATUS) & RX_DR) {
            nrf_read_payload();
            printf("Received: %c%c\n", buf[0], buf[1]);
            handle_zone(buf[0], buf[1]);
        }
        _delay_ms(100);
    }
}
