#include <avr/io.h>
#include <avr/interrupt.h>

#define TIME (F_CPU/256/256)/3

volatile int8_t DEBOUNCED;
volatile int16_t dice = 0;
/* Button state
 * First bit: A button is pressed
 * Second: Dice mode
 */
volatile uint8_t state = 0;

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
					PORTB &= ~15;
					PORTB += 6;
					dice = -1;
			} else if (dice < -3*TIME && (PINB & 15) != 15) {
					state &= ~_BV(1);
					dice = 0;
					PORTB &= ~15;
					PORTB |= state >> 4;
			}
		} else if (!(DEBOUNCED & _BV(4)) && (PINB & 15) > 1 && !(state & _BV(0))) {
			state |= _BV(0);
			PORTB--;
		} else if (!(DEBOUNCED & _BV(5)) && (PINB & 15) < 15 && !(state & _BV(0))) {
			state |= _BV(0);
			PORTB++;
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
		state |= _BV(1);
		if (dice > TIME) {
			state |= (PINB & 15) << 4;
			PORTB |= 15;
			dice = -1;
		} else
			dice++;
	} else if (dice > 0) {
		dice = 0;
		state &= ~_BV(1);
	}
}
