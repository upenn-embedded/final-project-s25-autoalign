/* main_receiver_spi1_min.c - Destynn Keuchel Minimized nRF24L01+ RX via SPI1 on ATmega328PB */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL
#define PAYLOAD_SIZE 5

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

uint8_t rx_addr[5]={0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t buf[PAYLOAD_SIZE];

static void spi1_init(void){ PRR1&=~(1<<PRSPI1);
    DDRE|=(1<<PE3)|(1<<PE2); DDRC|=(1<<PC1); DDRC&=~(1<<PC0);
    SPCR1=(1<<SPE1)|(1<<MSTR1)|(1<<SPR10); SPSR1&=~(1<<SPI2X1);
    CSN_LOW(); spi1_xfer(0x55); CSN_HIGH(); }

static void nrf_write(uint8_t r,uint8_t v){ CSN_LOW(); spi1_xfer(W_REG|r); spi1_xfer(v); CSN_HIGH(); }
static uint8_t nrf_read(uint8_t r){ uint8_t v; CSN_LOW(); spi1_xfer(R_REG|r); v=spi1_xfer(NOP); CSN_HIGH(); return v; }
static void nrf_read_payload(void){ CSN_LOW(); spi1_xfer(R_RX_PAY); for(int i=0;i<PAYLOAD_SIZE;i++) buf[i]=spi1_xfer(NOP); CSN_HIGH(); nrf_write(STATUS,RX_DR);} 

int main(void){ uart_init(); spi1_init();
    DDRD|=(1<<PD5)|(1<<PD6); CSN_HIGH(); CE_LOW(); _delay_ms(100);
    nrf_write(CONFIG,PRIM_RX|PWR_UP|EN_CRC); _delay_ms(5);
    nrf_write(EN_AA,0); nrf_write(EN_RXA,1); nrf_write(AW,3);
    nrf_write(CH,2); nrf_write(RF_SETUP,(1<<3)|(3<<1));
    CSN_LOW(); spi1_xfer(W_REG|RX_ADDR); for(int i=0;i<5;i++) spi1_xfer(rx_addr[i]); CSN_HIGH();
    nrf_write(RX_PW,PAYLOAD_SIZE);
    nrf_write(STATUS,RX_DR);
    CSN_LOW(); spi1_xfer(0xE1); CSN_HIGH(); // FLUSH_TX
    CSN_LOW(); spi1_xfer(0xE2); CSN_HIGH(); // FLUSH_RX
    CE_HIGH();
    while(1){ if(nrf_read(STATUS)&RX_DR){ nrf_read_payload(); printf("Received: \""); for(int i=0;i<PAYLOAD_SIZE;i++) putchar(buf[i]); printf("\"\n"); } _delay_ms(100);} }
