/* main_receiver_spi1_int.c
   nRF24L01+ RX via SPI1 on ATmega328PB, 2-byte payload, IRQ on PB1 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

#define PAYLOAD_SIZE 2

// control pins
#define CSN_DDR   DDRD
#define CSN_PORT  PORTD
#define CSN_PIN   PD7
#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CE_PIN    PB0
#define IRQ_DDR   DDRB
#define IRQ_PORT  PORTB
#define IRQ_PIN   PB1

static inline void CSN_LOW()  { CSN_PORT &= ~(1<<CSN_PIN); }
static inline void CSN_HIGH() { CSN_PORT |=  (1<<CSN_PIN); }
static inline void CE_HIGH()  { CE_PORT |=  (1<<CE_PIN); }
static inline void CE_LOW()   { CE_PORT &= ~(1<<CE_PIN); }

// SPI1 transfer
static uint8_t spi1_xfer(uint8_t d){
    SPDR1 = d;
    while (!(SPSR1 & (1<<SPIF1)));
    return SPDR1;
}

// nRF commands & regs
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

#define PRIM_RX   (1<<0)
#define PWR_UP    (1<<1)
#define EN_CRC    (1<<3)
#define RX_DR     (1<<6)

uint8_t rx_addr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t buf[PAYLOAD_SIZE];

static void spi1_init(void) {
    PRR1 &= ~(1<<PRSPI1);
    DDRE  |= (1<<PE3)|(1<<PE2);
    DDRC  |= (1<<PC1);
    DDRC  &= ~(1<<PC0);
    SPCR1  = (1<<SPE1)|(1<<MSTR1)|(1<<SPR10);
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
    // clear RX_DR
    nrf_write(STATUS, RX_DR);
}

// pin-change ISR for PB[7:0] -> PCINT0_vect
ISR(PCINT0_vect) {
    // IRQ is active low
    if (!(PINB & (1<<IRQ_PIN))) {
        // check RX_DR
        if (nrf_read(STATUS) & RX_DR) {
            nrf_read_payload();
            printf("IRQ: Received \"%c%c\"\r\n", buf[0], buf[1]);
        }
    }
}

int main(void) {
    uart_init();
    spi1_init();

    // config control pins
    CSN_DDR  |= (1<<CSN_PIN);
    CE_DDR   |= (1<<CE_PIN);
    IRQ_DDR  &= ~(1<<IRQ_PIN);
    IRQ_PORT |=  (1<<IRQ_PIN);  // pull-up on IRQ

    CSN_HIGH();
    CE_LOW();
    _delay_ms(100);

    // nRF24L01+ init
    nrf_write(CONFIG, PRIM_RX | PWR_UP | EN_CRC);
    _delay_ms(5);
    nrf_write(EN_AA,  0);
    nrf_write(EN_RXA, 1);
    nrf_write(AW,     3);
    nrf_write(CH,     2);
    nrf_write(RF_SETUP, (1<<3)|(3<<1));

    // set RX address
    CSN_LOW();
      spi1_xfer(W_REG | RX_ADDR);
      for (uint8_t i = 0; i < 5; i++)
          spi1_xfer(rx_addr[i]);
    CSN_HIGH();

    // set payload length
    nrf_write(RX_PW, PAYLOAD_SIZE);

    // clear flags & flush FIFOs
    nrf_write(STATUS, RX_DR);
    CSN_LOW(); spi1_xfer(0xE1); CSN_HIGH(); // FLUSH_TX
    CSN_LOW(); spi1_xfer(0xE2); CSN_HIGH(); // FLUSH_RX

    // enable pin-change interrupt on PB1 (PCINT1)
    PCICR  |= (1<<PCIE0);       // enable PCINT0..7
    PCMSK0 |= (1<<PCINT1);      // unmask PB1/PCINT1
    sei();                      // global interrupts on

    CE_HIGH();  // start listening

    // main loop empty; RX handled in ISR
    while (1) { }
}
