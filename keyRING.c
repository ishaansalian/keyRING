#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define TRIG PB1 // PB1 = pin 15
#define ECHO PB0 // PB0 = pin 14
#define RANGE_PER_CLOCK 1.098

// Function to initialize Timer 0
void timer0_init();

int main(void) {
    // Set up D6 as an output for the buzzer
    DDRD = 1 << DDD6;
    PORTD = 1 << PORTD3; // Set D3 as pull-up

    DDRB = 0;

    // Configure Timer 1 for volume control
    TCCR1A = 0;
    TCCR1B = 10;

    // Initialize Timer 0
    timer0_init();

    float rising_edge_clocks, falling_edge_clocks, echo_width_clocks;
    float distance, prev_distance = 0;

    // Set up TRIG pin as an output
    DDRB = 1 << TRIG;
    PORTB &= ~(1 << TRIG); // Set the TRIG pin low

    while (1) {
        TCNT0 = 0;              // Load counter with 0
        PORTB |= 1 << TRIG;     // Generate a 10 usec pulse on TRIG pin
        _delay_us(10);
        PORTB &= ~(1 << TRIG);  // Reset TRIG pin

        // Wait till the ECHO pulse goes high
        while (!(PINB & (1 << ECHO)))
            ;
        rising_edge_clocks = TCNT0; // Note the time

        // Now wait till the ECHO pulse goes low
        while (PINB & (1 << ECHO))
            ;
        falling_edge_clocks = TCNT0;

        if (falling_edge_clocks > rising_edge_clocks) {
            // Compute target range in cm
            echo_width_clocks = falling_edge_clocks - rising_edge_clocks;
            distance = echo_width_clocks * RANGE_PER_CLOCK;
        }

        if (abs(distance - prev_distance) > 10) {
            // Buzz the buzzer while the door is open
            while ((PIND & (1 << PIND3)) != 0) {
                while ((TIFR1 & (1 << TOV1)) == 0); // Wait for rollover flag
                TIFR1 |= 1 << TOV1;                 // Clear the rollover flag
                _delay_ms(5);
                PORTD ^= 1 << PORTD6; // Toggle D6 (buzzer)
            }
        }

        prev_distance = distance;
        // OLED_DisplayFloatNumber(distance / 2.54); // converting cm to in
        _delay_ms(500);
    }

    return 0; // never reached
}

void timer0_init(void) {
    TCCR0A = 0;   // Timer mode - normal
    TCCR0B = 0x05; // 1024 prescaler
}

// End of file
