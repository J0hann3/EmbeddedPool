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

	//set ouput CA4 for the first digit and register2 for the segments
	send_command(ADDRESS, 0x06, 0b01111111, 0x00, 2);
	// reset all the leds to off
	// set a, b, g, e, d => 0b00100101
	send_command(ADDRESS, 0x02, 0b01111111, 0b01011011, 2);
	while(1)
	{
	}
}
