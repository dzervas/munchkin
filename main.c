#include <avr/io.h>
#include <avr/interrupt.h>

volatile int8_t DEBOUNCED;
int8_t pressed = 0;

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
		if ((DEBOUNCED & _BV(4)) == 0 && (PINB & 15) > 1 && !pressed) {
			pressed = 1;
			PORTB--;
		} else if ((DEBOUNCED & _BV(5)) == 0 && (PINB & 15) < 15 && !pressed) {
			pressed = 1;
			PORTB++;
		} else if (DEBOUNCED == (_BV(4) | _BV(5)))
			pressed = 0;
	}

	return 0;
}

ISR(TIMER0_OVF_vect) {
	DEBOUNCED = PINB & (_BV(4) | _BV(5));
}
