#include <avr/io.h>
#include <util/delay.h>

#define MAX_4_BITS 15
#define MIN_4_BITS 0
#define BIT_1 0b00000001
#define BIT_2 0b00000010
#define BIT_3 0b00000100
#define BIT_4 0b00001000
#define LIGHT_BITS 0b00010111

static void _init_IO_data()
{
	DDRD &= ~((1 << PD2) | (1 << PD4)); //set the 2 switchs for input data
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);//set the four lights for output data
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); //init the four lights with the light off
}

static int _increment(int value)
{
	if (value + 1 > MAX_4_BITS)
		return MAX_4_BITS;
	value++;
	int res = PORTB & ~LIGHT_BITS; //save date other then the 4 lights
	res |= (value & BIT_1) | (value & BIT_2) | (value & BIT_3) | ((value & BIT_4) << 1); // add value of the 4 lights to the variable
	PORTB = res; //set the light according to the value
	return (value);
}

static int _decrement(int value)
{
	if (value - 1 < MIN_4_BITS)
		return MIN_4_BITS;
	value--;
	int res = PORTB & ~LIGHT_BITS; //save date other then the 4 lights
	res |= (value & BIT_1) | (value & BIT_2) | (value & BIT_3) | ((value & BIT_4) << 1); // add value of the 4 lights to the variable
	PORTB = res; //set the light according to the value
	return (value);
}

int main()
{
	short is_press_1 = 0;
	short is_press_2 = 0;
	short res = 0; //value to print with the LED
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
		_delay_ms(10); //wait before checking again
	}
}
