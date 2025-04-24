#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Pin assignments (using ATmega328PB PORTD)
#define DIR_PIN  PD0  // Direction control pin
#define STEP_PIN PD1  // Step control pin
#define MS1_PIN  PD4  // Microstep control pins
#define MS2_PIN  PD5
#define MS3_PIN  PD6

void setup_pins() {
    // Set all control pins as outputs
    DDRD |= (1 << DIR_PIN) | (1 << STEP_PIN) |
            (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);

    // Set direction initially (e.g., clockwise)
    PORTD |= (1 << DIR_PIN);

    // Set microstepping mode to 1/8: MS1 = HIGH, MS2 = HIGH, MS3 = LOW
    PORTD |= (1 << MS1_PIN);  // MS1 HIGH
    PORTD |= (1 << MS2_PIN);  // MS2 HIGH
    PORTD &= ~(1 << MS3_PIN); // MS3 LOW
}

void step_once() {
    // Send a single step pulse
    PORTD |= (1 << STEP_PIN);     // STEP high
    _delay_us(100);               // 100us high pulse
    PORTD &= ~(1 << STEP_PIN);    // STEP low
    _delay_us(100);               // 100us low pulse
}

int main(void) {
    setup_pins();
    for (int i = 0; i < 150; i++) {
            step_once();
        }

            _delay_ms(100);   
        }

    
