# Settings
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude
SIZE = avr-size
TARGET = attiny2313
CLOCK = 8000000

# Recommended build options
CFLAGS = -Wall -Wno-overflow -pedantic -std=c99 -Ofast -mmcu=$(TARGET) -DF_CPU=$(CLOCK)
OBJFLAGS = -j .text -j .data -O ihex
DUDEFLAGS = -p $(TARGET) -c avrisp -b 19200 -P /dev/ttyACM0
#DUDEFLAGS = -p $(TARGET) -c usbtiny -B 1

# Object files for the firmware
OBJECTS = main.o

# By default, build the firmware, but do not flash it
all: main.hex

# With this, you can flash the firmware
flash: main.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

# Housekeeping if you want it
clean:
	$(RM) */*.o *.o *.hex *.elf

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@
	$(SIZE) --mcu=$(TARGET) $@

# Main.elf requires additional objects to the firmware, not just main.o
main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# From C source to .o object file
%.o: %.c	
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
