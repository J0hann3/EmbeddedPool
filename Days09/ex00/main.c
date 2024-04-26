#include "tool.h"
#include "uart.h"
#include "i2c.h"
#include <stdbool.h>

#define A0 0
#define A1 1
#define A2 2

int main()
{
	i2c_init();
	uart_init();
	_delay_ms(100);
	// connection start with U4 in write mode
	i2c_start(TW_WRITE, 0b00100000 | (0 << A2) | (0 << A1) | (0 << A0));
	_delay_ms(100);

	// send commande to configure input/output mode (DDR)
	i2c_write(0x06);
	// set led d9 in outpout mode => 0
	i2c_write(0b11110111);
	i2c_stop();

	_delay_ms(100);

	i2c_start(TW_WRITE, 0b00100000 | (0 << A2) | (0 << A1) | (0 << A0));
	//send command to configure output port (PORT)
	i2c_write(0x02);
	//set led d9 to light on => 0
	i2c_write(0b11110111);
	i2c_stop();

	while(1)
	{
	}
}
