#include "tool.h"
#include "uart.h"

#define GREEN PD6
#define RED PD5
#define BLUE PD3

void red_led()
{
	SET(PORTD, RED);
	RESET(PORTD, GREEN);
	RESET(PORTD, BLUE);
}

void green_led()
{
	RESET(PORTD, RED);
	SET(PORTD, GREEN);
	RESET(PORTD, BLUE);
}

void blue_led()
{
	RESET(PORTD, RED);
	RESET(PORTD, GREEN);
	SET(PORTD, BLUE);
}

void yellow_led()
{
	SET(PORTD, RED);
	SET(PORTD, GREEN);
	RESET(PORTD, BLUE);
}

void cyan_led()
{
	RESET(PORTD, RED);
	SET(PORTD, GREEN);
	SET(PORTD, BLUE);
}

void magenta_led()
{
	SET(PORTD, RED);
	RESET(PORTD, GREEN);
	SET(PORTD, BLUE);
}

void white_led()
{
	SET(PORTD, RED);
	SET(PORTD, GREEN);
	SET(PORTD, BLUE);
}

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
		red_led();
		_delay_ms(1000);
		green_led();
		_delay_ms(1000);
		blue_led();
		_delay_ms(1000);
		yellow_led();
		_delay_ms(1000);
		cyan_led();
		_delay_ms(1000);
		magenta_led();
		_delay_ms(1000);
		white_led();
		_delay_ms(1000);
	}
}
