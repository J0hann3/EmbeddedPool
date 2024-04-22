#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>

#define MASK_NO_LED 0b00010111
#define ADDRESS_COUNTER_SELECTED 0
#define ADDRESS_COUNTER 1

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
	RESET(DDRD, PD4);

	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);
	uint4_t	value;
	uint8_t is_press1 = 0;
	uint8_t is_press2 = 0;

	uint8_t address = eeprom_read_byte(ADDRESS_COUNTER_SELECTED);
	if (address > 4)
	{
		eeprom_write_byte(ADDRESS_COUNTER_SELECTED, 0);
		eeprom_write_byte((uint8_t*)(ADDRESS_COUNTER_SELECTED + 1), 0);
		eeprom_write_byte((uint8_t*)(ADDRESS_COUNTER_SELECTED + 2), 0);
		eeprom_write_byte((uint8_t*)(ADDRESS_COUNTER_SELECTED + 3), 0);
		eeprom_write_byte((uint8_t*)(ADDRESS_COUNTER_SELECTED + 4), 0);
	}

	while(1)
	{
		address = eeprom_read_byte(ADDRESS_COUNTER_SELECTED);
		if (is_press1 == 0 && (PIND & (1 << PD2)) == 0)
		{
			is_press1 = 1;
			value.val = eeprom_read_byte((uint8_t*)(address + ADDRESS_COUNTER));
			value.val++;
			eeprom_write_byte((uint8_t*)(address + ADDRESS_COUNTER), value.val);
			_delay_ms(200);
		}
		else if (is_press1 == 1 && (PIND & (1 << PD2)) != 0)
			is_press1 = 0;
		if (is_press2 == 0 && (PIND & (1 << PD4)) == 0)
		{
			is_press2 = 1;
			address = (address + 1) % 4;
			eeprom_write_byte(ADDRESS_COUNTER_SELECTED, address);
			_delay_ms(200);
		}
		else if (is_press2 == 1 && (PIND & (1 << PD4)) != 0)
			is_press2 = 0;
		value.val = eeprom_read_byte((uint8_t*)(address + ADDRESS_COUNTER));
		print_value(value);
	}
}
