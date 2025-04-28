/* transmitter_min.c - Destynn Keuchel Minimized TX nRF24L01+ via SPI0 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL
#define PAYLOAD_SIZE 5

// Pins: CSN=PB1, CE=PB0
#define CSN_LOW()  PORTB&=~(1<<PB1)
#define CSN_HIGH() PORTB|=(1<<PB1)
#define CE_LOW()   PORTB&=~(1<<PB0)
#define CE_HIGH()  PORTB|=(1<<PB0)

static uint8_t spi_xfer(uint8_t d){SPDR0=d;while(!(SPSR0&(1<<SPIF)));return SPDR0;}
static void spi_init(void){
    DDRB|=(1<<PB3)|(1<<PB5)|(1<<PB2)|(1<<PB1)|(1<<PB0);
    DDRB&=~(1<<PB4);
    SPCR0=(1<<SPE)|(1<<MSTR)|(1<<SPR0);
    SPSR0&=~(1<<SPI2X);
    CSN_HIGH();CE_LOW();
}

static void nrf_cmd(uint8_t cmd,const uint8_t*d,uint8_t n){
    CSN_LOW();spi_xfer(cmd);
    while(n--)spi_xfer(*d++);
    CSN_HIGH();
}
static uint8_t nrf_reg(uint8_t r){uint8_t v;CSN_LOW();spi_xfer(r);v=spi_xfer(0xFF);CSN_HIGH();return v;}

uint8_t tx_addr[5]={0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t txp[PAYLOAD_SIZE]={'H','e','l','l','o'};

int main(void){
    uart_init();spi_init();_delay_ms(100);
    // setup: PWR_UP+CRC, AW=5, CH=2, 2Mbps, no ACK, TX_ADDR
    nrf_cmd(0x20|0x00,(uint8_t[]){(1<<1)|(1<<3)},1);_delay_ms(5);
    nrf_cmd(0x20|0x03,(uint8_t[]){3},1);
    nrf_cmd(0x20|0x05,(uint8_t[]){2},1);
    nrf_cmd(0x20|0x06,(uint8_t[]){(1<<3)|(3<<1)},1);
    nrf_cmd(0x20|0x01,(uint8_t[]){0},1);
    nrf_cmd(0x20|0x10,tx_addr,5);
    // debug: post-config
    printf("%02X %02X %02X %02X %02X\n",nrf_reg(0),nrf_reg(1),nrf_reg(3),nrf_reg(5),nrf_reg(6));
    while(1){
        printf("Loop\n");
        // send
        nrf_cmd(0xE1,0,0); // FLUSH_TX
        CSN_LOW();spi_xfer(0xA0);
        for(int i=0;i<PAYLOAD_SIZE;i++)spi_xfer(txp[i]);
        CSN_HIGH();
        CE_HIGH();_delay_ms(10);CE_LOW();
        printf("Sent \"%.*s\"\n",PAYLOAD_SIZE,txp);
        uint8_t s=nrf_reg(7),o=nrf_reg(8),f=nrf_reg(0x17);
        printf("%02X %d %d %d %d %d %d %d\n",s,s&0x20,s&0x10,s&0x40,(o>>4)&0xF,o&0xF,f&0x20,f&0x10);
        nrf_cmd(0x20|7,(uint8_t[]){0x70},1); // clear IRQs
        _delay_ms(500);
    }
}
