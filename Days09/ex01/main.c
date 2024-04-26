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

void print_value_led(uint8_t value)
{
	value = (~value & 0b00000111) << 1;
	send_command(ADDRESS, 0x02, value, NONE, 1);
}

int main()
{
	i2c_init();
	uart_init();

	//set input for switch and ouput for leds
	send_command(ADDRESS, 0x06, 0b11110001, NONE, 1);
	// reset all the leds to off
	send_command(ADDRESS, 0x02, 0b11111111, NONE, 1);

	uint8_t cpt = 0;
	uint8_t is_press = 0;
	while(1)
	{
		//check the value of the sw3
		if (is_press == 0 && (read_value(ADDRESS, 0x00) & (1 << SW3)) == 0)
		{
			is_press = 1;
			cpt++;
			if (cpt == 8)
				cpt = 0;
			print_value_led(cpt);
			_delay_ms(150);
		}
		if (is_press == 1 && (read_value(ADDRESS, 0x00) & (1 << SW3)) != 0)
			is_press = 0;
	}
}
