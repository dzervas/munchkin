# munchkin
A binary counter for AVR to keep track of munchkin levels

It has 4 leds to show the current count (1-16)

It has 2 buttons to add/sub to the count

If you press both buttons for ~2s it will show a random number 1-6 (dice)


It uses the internal oscillator (8MHz) so the timming might (should) be kinda off,
but it won't affect you much.

## Wiring

Note: It uses the reset pin (PB5) on ATTiny85,
so the fuses will disable reprogramming.
Use with care, as you need a [high voltage AVR programmer](http://www.instructables.com/id/AVR-High-voltage-programming-Fuses-rescue/)
to reprogram it and change the fuses back.

PB0-3: Indication leds (add resistors to limit brightness)
PB4: -1 button
PB5: +1 button
