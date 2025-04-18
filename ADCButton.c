/* integrated_transmitter.c - Combined button detection with nRF24L01+ transmission */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL
#define PAYLOAD_SIZE 2

// Pins: CSN=PB1, CE=PB0
#define CSN_LOW()  PORTB&=~(1<<PB1)
#define CSN_HIGH() PORTB|=(1<<PB1)
#define CE_LOW()   PORTB&=~(1<<PB0)
#define CE_HIGH()  PORTB|=(1<<PB0)

// nRF24L01+ Radio Functions
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

// Send a payload over nRF24L01+
static void send_payload(uint8_t *payload, uint8_t size) {
    nrf_cmd(0xE1, 0, 0); // FLUSH_TX
    CSN_LOW();
    spi_xfer(0xA0);
    for(int i=0; i<size; i++) spi_xfer(payload[i]);
    CSN_HIGH();
    CE_HIGH();
    _delay_ms(10);
    CE_LOW();
    
    // Clear IRQs
    nrf_cmd(0x20|7, (uint8_t[]){0x70}, 1);
}

// Read from ADC
uint16_t readADC(int channel) {
    ADMUX = (1 << REFS0) | (channel & 0x0F); // AVcc ref + select ADC channel
    ADCSRA |= (1 << ADSC);                   // Start conversion
    while (ADCSRA & (1 << ADSC));            // Wait for conversion
    return ADC;
}

// Red panel: 8 buttons
uint8_t detectRedButton(int adcVal) {
    if (adcVal > 70 && adcVal < 140)  return 1;
    if (adcVal > 150 && adcVal < 230) return 2;
    if (adcVal > 242 && adcVal < 320) return 3;
    if (adcVal > 330 && adcVal < 410) return 4;
    if (adcVal > 420 && adcVal < 500) return 5;
    if (adcVal > 520 && adcVal < 610) return 6;
    if (adcVal > 640 && adcVal < 730) return 7;
    if (adcVal > 790 && adcVal < 880) return 8;
    return 0;
}

// White panel: 4 buttons
uint8_t detectWhiteButton(int adcVal) {
    if (adcVal > 150 && adcVal < 200) return 1;
    if (adcVal > 300 && adcVal < 400) return 2;
    if (adcVal > 500 && adcVal < 600) return 3;
    if (adcVal > 700 && adcVal < 800) return 4;
    return 0;
}

// Yellow panel: 4 buttons
uint8_t detectYellowButton(int adcVal) {
    if (adcVal > 150 && adcVal < 200) return 1;
    if (adcVal > 300 && adcVal < 400) return 2;
    if (adcVal > 500 && adcVal < 600) return 3;
    if (adcVal > 700 && adcVal < 800) return 4;
    return 0;
}

// Global variables
uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t txp[PAYLOAD_SIZE] = {'R', '7'}; // Default payload, will be changed based on button press

int main(void) {
    // Initialize UART and SPI
    uart_init();
    spi_init();
    _delay_ms(100);
    
    // Setup nRF24L01+: PWR_UP+CRC, AW=5, CH=2, 2Mbps, no ACK, TX_ADDR
    nrf_cmd(0x20|0x00, (uint8_t[]){(1<<1)|(1<<3)}, 1); _delay_ms(5);
    nrf_cmd(0x20|0x03, (uint8_t[]){3}, 1);
    nrf_cmd(0x20|0x05, (uint8_t[]){2}, 1);
    nrf_cmd(0x20|0x06, (uint8_t[]){(1<<3)|(3<<1)}, 1);
    nrf_cmd(0x20|0x01, (uint8_t[]){0}, 1);
    nrf_cmd(0x20|0x10, tx_addr, 5);
    
    // Bullseye button setup
    DDRC &= ~(1<<DDC1);  // PC1 as input
    PORTC |= (1 << PC1); // Enable pull-up on PC1
    
    // ADC setup
    ADMUX = (1 << REFS0); // AVcc reference
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 128

    int redPressed = 0, whitePressed = 0, yellowPressed = 0, bullseyePressed = 0;

    while (1) {
        // Check for button presses
        int redVal = readADC(0);    // PC0
        int whiteVal = readADC(4);  // PC4
        int yellowVal = readADC(5); // PC5
        
        // Checking bullseye button (active low)
        if (!(PINC & (1 << PC1)) && !bullseyePressed) {
            printf("Bullseye\n");
            // Set payload to "BE" for bullseye
            txp[0] = 'B';
            txp[1] = 'E';
            // Send over nRF24L01+
            send_payload(txp, PAYLOAD_SIZE);
            printf("Sent \"%.*s\"\n", PAYLOAD_SIZE, txp);
            bullseyePressed = 1;
        } else if ((PINC & (1 << PC1))) {
            bullseyePressed = 0;
        }

        // Check red panel buttons
        int redButton = detectRedButton(redVal);
        if (redButton > 0 && !redPressed) {
            txp[0] = 'R';
            txp[1] = '0' + redButton; // Convert to ASCII
            send_payload(txp, PAYLOAD_SIZE);
            printf("Sent \"%.*s\"\n", PAYLOAD_SIZE, txp);
            redPressed = 1;
        } else if (redButton == 0) {
            redPressed = 0;
        }

        // Check white panel buttons
        int whiteButton = detectWhiteButton(whiteVal);
        if (whiteButton > 0 && !whitePressed) {
            txp[0] = 'W';
            txp[1] = '0' + whiteButton; // Convert to ASCII
            send_payload(txp, PAYLOAD_SIZE);
            printf("Sent \"%.*s\"\n", PAYLOAD_SIZE, txp);
            whitePressed = 1;
        } else if (whiteButton == 0) {
            whitePressed = 0;
        }

        // Check yellow panel buttons
        int yellowButton = detectYellowButton(yellowVal);
        if (yellowButton > 0 && !yellowPressed) {
            txp[0] = 'Y';
            txp[1] = '0' + yellowButton; // Convert to ASCII
            send_payload(txp, PAYLOAD_SIZE);
            printf("Sent \"%.*s\"\n", PAYLOAD_SIZE, txp);
            yellowPressed = 1;
        } else if (yellowButton == 0) {
            yellowPressed = 0;
        }

        // Small delay to avoid excessive CPU usage and debounce
        _delay_ms(2);
    }
}