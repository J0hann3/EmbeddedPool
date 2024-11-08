CC=avr-gcc
F_CPU=16000000
UART_BAUDRATE=115200
CFLAGS=-mmcu=atmega328p -D F_CPU=$(F_CPU) -D UART_BAUDRATE=$(UART_BAUDRATE) -Os -MMD

NAME=main

SRC=main.c \
	uart.c \
	i2c.c \
	slave.c \
	master.c

OBJ=$(SRC:.c=.o)

HEADER = $(patsubst %.o,%.d, $(OBJ))

all: hex flash

-include $(HEADER)

hex: $(NAME).hex

$(NAME).hex: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME).bin
	avr-objcopy -O ihex $(NAME).bin $(NAME).hex

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

flash:
	avrdude -p m328p -c arduino -P /dev/ttyUSB1 -b $(UART_BAUDRATE) -u -U flash:w:$(NAME).hex:i
	terminator -e 'screen -S ex00 /dev/ttyUSB1 $(UART_BAUDRATE)'&
	avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b $(UART_BAUDRATE) -u -U flash:w:$(NAME).hex:i
	screen -S ex01 /dev/ttyUSB0 $(UART_BAUDRATE)

clean:
	rm -rf $(NAME).bin $(NAME).hex
	rm -rf $(OBJ)
	rm -rf $(HEADER)

.PHONY : clean all hex flash clean