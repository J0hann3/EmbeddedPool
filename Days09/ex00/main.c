#include "tool.h"
#include "uart.h"
#include "i2c.h"
#include <stdbool.h>

#define A0 0
#define A1 1
#define A2 2

void send_command(uint8_t address, uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t nb_arg)
{
	// connection start with U4 in write mode
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

int main()
{
	i2c_init();
	uart_init();
	_delay_ms(100);
	// send commande to configure input/output mode (DDR)
	// set led d9 in outpout mode => 0
	send_command(0b00100000 | (0 << A2) | (0 << A1) | (0 << A0), 0x06, 0b11110111, 0, 1);

	while(1)
	{
		//send command to configure output port (PORT)
		//set led d9 to light on => 0
		send_command(0b00100000 | (0 << A2) | (0 << A1) | (0 << A0), 0x02, 0b11110111, 0, 1);

		_delay_ms(500);

		//send command to configure output port (PORT)
		//set led d9 to light off => 1
		send_command(0b00100000 | (0 << A2) | (0 << A1) | (0 << A0), 0x02, 0b11111111, 0, 1);
		
		_delay_ms(500);
	}
}
