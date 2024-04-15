CC=avr-gcc
CFLAGS=-mmcu=atmega328p -D F_CPU=16000000 -Os

all: hex flash

hex:
	$(CC) $(CFLAGS) main.c -o main.bin 
	avr-objcopy -O ihex main.bin main.hex

flash:
	avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b 115200 -u -U flash:w:main.hex:i

clean:
	rm main.bin main.hex