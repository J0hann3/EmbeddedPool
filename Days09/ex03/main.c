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

	_delay_ms(100);
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

	_delay_ms(100);
	return res;
}

int main()
{
	i2c_init();

	//set ouput CA4 for the first digit and register2 for the segments
	send_command(ADDRESS, 0x06, 0b01111111, 0x00, 2);
	uint8_t cpt = 0;
	while(1)
	{
		switch(cpt)
		{
			case 0:
				send_command(ADDRESS, 0x02, 0b01111111, ZERO, 2);
				break;
			case 1:
				send_command(ADDRESS, 0x02, 0b01111111, ONE, 2);
				break;
			case 2:
				send_command(ADDRESS, 0x02, 0b01111111, TWO, 2);
				break;
			case 3:
				send_command(ADDRESS, 0x02, 0b01111111, THREE, 2);
				break;
			case 4:
				send_command(ADDRESS, 0x02, 0b01111111, FOUR, 2);
				break;
			case 5:
				send_command(ADDRESS, 0x02, 0b01111111, FIVE, 2);
				break;
			case 6:
				send_command(ADDRESS, 0x02, 0b01111111, SIX, 2);
				break;
			case 7:
				send_command(ADDRESS, 0x02, 0b01111111, SEVEN, 2);
				break;
			case 8:
				send_command(ADDRESS, 0x02, 0b01111111, HEIGHT, 2);
				break;
			case 9:
				send_command(ADDRESS, 0x02, 0b01111111, NINE, 2);
				break;
			default:
				send_command(ADDRESS, 0x02, 0b01111111, 0, 2);
		}
		_delay_ms(1000);
		cpt++;
		if (cpt == 10)
			cpt = 0;
	}
}
