#include "tool.h"

#define MASK_NONE_LED 0b00010111

struct bit_field
{
	uint8_t value : 4;
};

volatile struct bit_field value = {0};

static void _init_switch1()
{
	RESET(DDRD, PD2); //set switch to input

	//Activate global interrupt
	SET(SREG, 7);

	//Active INT0
	SET(EIMSK, INT0);

	// The falling edge of INT0 generates an interrupt request.
	SET(EICRA, ISC01);
	RESET(EICRA, ISC00);
}

static void _init_switch2()
{
	RESET(DDRD, PD4); //set switch to input

	//Activate global interrupt
	SET(SREG, 7);

	//Activate pin PD4 for interrupt when pin change state
	SET(PCMSK2, PCINT20);
	SET(PCICR, PCIE2);
}

static void _print()
{
	char res;
	res = PORTB & ~MASK_NONE_LED; //save info of PORTB before changing LED state
	res |= (value.value & 1) | (value.value & 2) | (value.value & 4) | ((value.value & 8) << 1);
	PORTB = res;
}

ISR(PCINT2_vect)
{
	static uint8_t is_press = 0;
	is_press = ~is_press;
	if (is_press == 0)
		return;
	value.value--;
	_print();

}

ISR(INT0_vect)
{
	value.value++;
	_print();
}

int main()
{
	SET(DDRB, PB0); //set light to output
	SET(DDRB, PB1); //set light to output
	SET(DDRB, PB2); //set light to output
	SET(DDRB, PB4); //set light to output
	_init_switch1();
	_init_switch2();
	while (1)
	{}
}