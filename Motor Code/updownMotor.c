#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define STEP_PIN PD2
#define DIR_PIN PD3

void setup_pins() {
    // Set PD2 (STEP) and PD3 (DIR) as outputs
    DDRD |= (1 << STEP_PIN) | (1 << DIR_PIN);
    
    // Initial state: direction forward
    PORTD |= (1 << DIR_PIN);
}

void step_once() {
    // Create a single step pulse
    PORTD |= (1 << STEP_PIN);   // STEP high
    _delay_us(100);             // hold high for 100 µs
    PORTD &= ~(1 << STEP_PIN);  // STEP low
    _delay_us(100);             // hold low for 100 µs
}

int main(void) {
    setup_pins();

    while (1) {
        for (int i = 0; i < 200; i++) { // 200 steps = 1 rotation for 1.8° stepper
            step_once();
        }

        _delay_ms(10);

        // Reverse direction
//        PORTD ^= (1 << DIR_PIN); // toggle direction
//        _delay_ms(1000);
    }
}
