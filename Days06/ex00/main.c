#include "tool.h"
#include "uart.h"

#define GREEN PD6
#define RED PD5
#define BLUE PD3

int main()
{
	//set led rgb to output
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	//reset all the led rgb
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	while(1)
	{
		SET(PORTD, RED); //switch on red led
		_delay_ms(1000);
		RESET(PORTD, RED);//switch off red led
		SET(PORTD, GREEN); //switch on green led
		_delay_ms(1000);
		RESET(PORTD, GREEN); //switch off green led
		SET(PORTD, BLUE); //switch on blue led
		_delay_ms(1000);
		RESET(PORTD, BLUE); //switch off blue led
	}
}
