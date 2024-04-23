#include "tool.h"
#include "uart.h"

#define GREEN PD6
#define RED PD5
#define BLUE PD3

int main()
{
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	while(1)
	{
		SET(PORTD, RED);
		_delay_ms(1000);
		RESET(PORTD, RED);
		SET(PORTD, GREEN);
		_delay_ms(1000);
		RESET(PORTD, GREEN);
		SET(PORTD, BLUE);
		_delay_ms(1000);
		RESET(PORTD, BLUE);
	}
}
