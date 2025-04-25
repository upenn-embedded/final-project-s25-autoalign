/* transmitter_min.c - Destynn Keuchel Minimized TX nRF24L01+ via SPI0 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define STEPPER_PORT   PORTD
#define STEPPER_DDR    DDRD
#define STEPPER_MASK   ((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5))
#define STEP_SHIFT     PD2

const uint8_t fullStep[4] = {
    0b0011,
    0b0110,
    0b1100,
    0b1001
};

static uint8_t  idx = 0;
static int8_t   dir =  1;     // 1 = forward, -1 = reverse
//static uint16_t delay_ms =  3; // tune for speed

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
        _delay_ms(3);
        idx = (idx + 3) & 0x03;  // same as idx–1 mod 4
    }
}

int main(void) {
    STEPPER_DDR |= STEPPER_MASK;
    while (1) {
        
        backward(800);
        forward(800);
        _delay_ms(1000);
    }
}