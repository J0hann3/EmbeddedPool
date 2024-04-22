#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>

#define MASK_NO_LED 0b00010111
#define ADDRESS 0

typedef struct s_value
{
	uint8_t val : 4;
} uint4_t;

void print_value(uint4_t value)
{
	char res;
	res = PORTB & ~MASK_NO_LED; //save info of PORTB before changing LED state
	res |= (value.val & 1) | (value.val & 2)
			| (value.val & 4) | ((value.val & 8) << 1);
	PORTB = res;
}


int main()
{
	RESET(DDRD, PD2);
	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);
	uint4_t	value;
	uint8_t is_press = 0;

	while(1)
	{
		if (is_press == 0 && (PIND & (1 << PD2)) == 0)
		{
			is_press = 1;
			value.val = eeprom_read_byte(ADDRESS);
			value.val++;
			eeprom_write_byte(ADDRESS, value.val);
			_delay_ms(200);
		}
		else if (is_press == 1 && (PIND & (1 << PD2)) != 0)
			is_press = 0;
		value.val = eeprom_read_byte(ADDRESS);
		print_value(value);
	}
}
