# makefile for AVR ATmega328P projects using the Arduino Uno
# Original Author: Professor D. McLaughlin
# Contributors: Aryaman Ghura, Ishaan Salian

# Specify the com port (windows) or USB port (macOS)
SERIALPORT = /dev/tty.usbmodem1101

# Specify the name of source code here:
SOURCEFILE = keyRING.c

# Don't change anything below
CLOCKSPEED = 16000000	
PROGRAMMER = Arduino

begin: main.hex

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=atmega328p main.elf

main.elf: $(SOURCEFILE)
	avr-gcc -Wall -Os -DF_CPU=$(CLOCKSPEED) -mmcu=atmega328p -o main.elf $(SOURCEFILE)

flash: begin
	avrdude -c $(PROGRAMMER) -b 115200 -P $(SERIALPORT) -p atmega328p -U flash:w:main.hex:i
