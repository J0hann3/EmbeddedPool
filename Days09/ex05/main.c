#include "tool.h"
#include "uart.h"
#include "i2c.h"
#include <stdbool.h>

#define A0 0
#define A1 1
#define A2 2
#define NONE 0
#define SW3 0
#define ADDRESS (0b00100000 | (0 << A2) | (0 << A1) | (0 << A0))
#define ZERO	0b00111111
#define ONE		0b00000110
#define TWO		0b01011011
#define THREE	0b01001111
#define FOUR	0b01100110
#define FIVE	0b01101101
#define SIX		0b01111101
#define SEVEN	0b00000111
#define HEIGHT	0b01111111
#define NINE	0b01101111

volatile uint16_t cpt;

void send_command(uint8_t address, uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t nb_arg)
{
	i2c_start(TW_WRITE, address);
	//send command
	i2c_write(command);
	//send arg
	i2c_write(arg1);
	if (nb_arg == 2)
		i2c_write(arg2);
	i2c_stop();
}

uint8_t read_value(uint8_t address, uint8_t command)
{
	i2c_start(TW_WRITE, address);
	//send command
	i2c_write(command);

	//restart in read mode
	i2c_restart(TW_READ, address);
	//receive arg and send nack to stop after
	uint8_t res = i2c_read(NACK);
	i2c_stop();

	return res;
}

void init_timer()
{
	sei();
	// set waveform generation to mode 4 CTC
	RESET(TCCR1A, WGM10);
	RESET(TCCR1A, WGM11);
	SET(TCCR1B, WGM12);
	RESET(TCCR1B, WGM13);

	// set prescaler to 256
	RESET(TCCR1B, CS10);
	RESET(TCCR1B, CS11);
	SET(TCCR1B, CS12);

	// set interrupt for OCR1A
	SET(TIMSK1, OCIE0A);

	OCR1A = 62500;//every 1s => 1Hz
	// 16 000 000 / 1 / 256 = 62 500
}

ISR(TIMER1_COMPA_vect)
{
	cpt++;
	if (cpt == 1000)
		cpt = 0;
}

void print_one_digit(uint8_t nb, uint8_t unity)
{
	send_command(ADDRESS, 0x02, 0b11111111, 0, 2);
	switch(nb)
	{
		case 0:
			send_command(ADDRESS, 0x02, unity, ZERO, 2);
			break;
		case 1:
			send_command(ADDRESS, 0x02, unity, ONE, 2);
			break;
		case 2:
			send_command(ADDRESS, 0x02, unity, TWO, 2);
			break;
		case 3:
			send_command(ADDRESS, 0x02, unity, THREE, 2);
			break;
		case 4:
			send_command(ADDRESS, 0x02, unity, FOUR, 2);
			break;
		case 5:
			send_command(ADDRESS, 0x02, unity, FIVE, 2);
			break;
		case 6:
			send_command(ADDRESS, 0x02, unity, SIX, 2);
			break;
		case 7:
			send_command(ADDRESS, 0x02, unity, SEVEN, 2);
			break;
		case 8:
			send_command(ADDRESS, 0x02, unity, HEIGHT, 2);
			break;
		case 9:
			send_command(ADDRESS, 0x02, unity, NINE, 2);
			break;
		default:
			send_command(ADDRESS, 0x02, unity, 0, 2);
	}
}

void print_number(uint16_t nb)
{
	uint8_t start = 0;

	if (nb / 1000 != 0 || start)
	{
		print_one_digit(nb / 1000, 0b11101111);
		start = 1;
	}
	nb = nb % 1000;
	if (nb / 100 != 0 || start)
	{
		print_one_digit(nb / 100, 0b11011111);
		start = 1;
	}
	nb = nb % 100;
	if (nb / 10 != 0 || start)
	{
		print_one_digit(nb / 10, 0b10111111);
		start = 1;
	}
	nb = nb % 10;
	if (nb != 0 || start)
	{
		print_one_digit(nb, 0b01111111);
		start = 1;
	}
}

int main()
{
	init_timer();
	uart_init();
	i2c_init();

	cpt = 0;
	//set ouput CA4 for the first digit and register2 for the segments
	send_command(ADDRESS, 0x06, 0b00001111, 0x00, 2);
	while(1)
	{
		print_number(cpt);
	}
}
