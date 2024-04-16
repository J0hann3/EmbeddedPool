#include <avr/io.h>
#include <util/delay.h>
#include "tool.h"

#define MAX 100
#define MIN 10

static void _init_IO_data()
{
	//set the 2 switchs for input data
	RESET(DDRD, PD2);
	RESET(DDRD, PD4);

	//set light to output
	SET(DDRB, PB1);

	// time register
	RESET(TCCR1A, WGM10);	 // mode 14 Fast PWM
	SET(TCCR1A, WGM11);		 // mode 14 Fast PWM
	SET(TCCR1B, WGM12);		 // mode 14 Fast PWM
	SET(TCCR1B, WGM13);		 // mode 14 Fast PWM

	RESET(TCCR1A, COM1A0);	// set pin OC1A=0 on Compare Match with OCR1A and 1 on BOTTOM
	SET(TCCR1A, COM1A1);	// set pin OC1A=0 on Compare Match with OCR1A and 1 on BOTTOM

	SET(TCCR1B, CS12);		// divise clock by 256
	RESET(TCCR1B, CS11);	// divise clock by 256
	RESET(TCCR1B, CS10);	// divise clock by 256

	OCR1A = 62500 * 0.1; //set value for compare match

	ICR1 = 62500; //set TOP value
}

static int _increment(int value)
{
	if (value + 10 > MAX)
		return MAX;
	value += 10;
	OCR1A = 62500 / 100. * value ;
	return (value);
}


static int _decrement(int value)
{
	if (value - 10 < MIN)
		return MIN;
	value -= 10;
	OCR1A = 62500 / 100. * value ;
	return (value);
}

int main()
{
	short is_press_1 = 0;
	short is_press_2 = 0;
	int res = 10;
	_init_IO_data();

	while(1)
	{
		if (is_press_1 == 0 && (PIND & (1 << PD2)) == 0) // check if PortD2 input data is actived and the button was release
		{
			res = _increment(res);
			is_press_1 ^= 1; //save the state button press
		}
		else if (is_press_1 == 1 && (PIND & (1 << PD2)) != 0) // check if PortD2 input data is desactived and the button was press
			is_press_1 ^= 1; //save the state button release
		if (is_press_2 == 0 && (PIND & (1 << PD4)) == 0) // check if PortD4 input data is actived and the button was release
		{
			res = _decrement(res);
			is_press_2 ^= 1; //save the state button press
		}
		else if (is_press_2 == 1 && (PIND & (1 << PD4)) != 0) // check if PortD4 input data is desactived and the button was press
			is_press_2 ^= 1; //save the state button release
		_delay_ms(10);
	}
}
