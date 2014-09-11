#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define TIME (F_CPU/256/256)/4

/* Button state
 * First bit: A button is pressed
 * Second: Dice mode
 * Third: Last operation before pressing second button
 */
volatile uint8_t state = 0;
volatile int8_t DEBOUNCED;
volatile int16_t dice = 0;

int main() {
	/* Set up I/O */
	DDRB = 0;
	DDRB &= ~_BV(4) & ~_BV(5);
	DDRB |= _BV(0) | _BV(1) | _BV(2) | _BV(3);

	PORTB = 1 | _BV(4) | _BV(5);
	DEBOUNCED = PINB & (_BV(4) | _BV(5));

	/* Set up timers */
	TIMSK |= _BV(TOIE0);
	TCCR0B |= _BV(CS02);
	sei();

	while (1) {
		if (state & _BV(1)) {
			if (dice < -TIME && (PINB & 15) == 15) {
				/* Show the dice's random number */
				PORTB &= ~15;
				PORTB += 1 + (rand() % 6);
				dice = -1;
			} else if (dice < -4*TIME && (PINB & 15) != 15) {
				/* Show the level prior to the dice roll */
				state &= ~_BV(1);
				dice = 0;
				PORTB &= ~15;
				PORTB |= (state & (15 << 4)) >> 4;
				state &= ~(15 << 4);
			}
		} else if (!(DEBOUNCED & _BV(4)) && !(state & _BV(0))) {
			state |= _BV(0);
			state &= ~_BV(2);
			if ((PINB & 15) > 1)
				PORTB--;
			else
				PORTB |= 15;
		} else if (!(DEBOUNCED & _BV(5)) && !(state & _BV(0))) {
			state |= _BV(0);
			state |= _BV(2);
			if ((PINB & 15) < 15)
				PORTB++;
			else
				PORTB &= ((15 << 4) | 1);
		} else if (DEBOUNCED == (_BV(4) | _BV(5)))
			state &= ~_BV(0);
	}

	return 0;
}

ISR(TIMER0_OVF_vect) {
	DEBOUNCED = PINB & (_BV(4) | _BV(5));

	if (dice < 0)
		dice--;

	if (!DEBOUNCED) {
		/* Both pressed, possible dice mode, start counting */
		if (dice > TIME && !(state & (15 << 4))) {
			/* Avoid micro-movements when trying to press both buttons */
			if (state & _BV(0) && state & _BV(2))
				state |= ((PINB & 15) - 1) << 4;
			else if (state & _BV(0))
				state |= ((PINB & 15) + 1) << 4;
			else
				state |= ((PINB & 15) << 4);

			/* Indicate the start of dice mode*/
			PORTB |= 15;
			dice = -1;
		} else {
			state |= _BV(1);
			dice++;
		}
	} else if (dice > 0) {
		/* False dice mode */
		dice = 0;
		state &= ~_BV(1);
	}
}
