#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define F_CPU 16000000UL  // 16 MHz clock frequency


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

int main(void) {
    init_pins_y();
    init_pins_x();

    uint8_t stepy = 0;
    uint8_t stepx = 0;
    
    while (1) {
        for(int i=0; i<10;i++){
            step_motor_y(stepy++);
            step_motor_x(stepx++);
            _delay_ms(10000); // Adjust for motor speed
        }
        for(int j=0; j<10;j++){
            
            step_motor_y(stepy--);
            step_motor_x(stepx--);
            _delay_ms(10000); // Adjust for motor speed
        }
    }

    return 0;
}
