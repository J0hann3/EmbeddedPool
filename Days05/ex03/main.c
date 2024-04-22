#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <stdbool.h>
// #include <strings.h>

#define MAGIC_NUMBER 42
#define MASK_FREE 0b10000000
#define ID 1
#define LENGTH 1
#define DATA 1

// bool safe_eeprom_read(void *buffer, size_t offset, size_t length, uint16_t id)
// {
// 	if (1 + offset + length >= 1024)
// 		return false;
// 	if ((eeprom_read_byte((uint8_t *)offset) & ~MASK_FREE) != MAGIC_NUMBER)
// 		return false;
// 	eeprom_read_block(buffer, (uint16_t *)(offset + 1), length);
// 	return true;
// }

bool safe_eeprom_write(void *buffer, size_t offset, size_t length, uint16_t id)
{
	if (DATA + offset + length >= 1024)
		return false;
	uint8_t magic_number = MAGIC_NUMBER;
	eeprom_update_block(&magic_number, (uint8_t *)offset, 1);
	eeprom_update_word((uint16_t *)offset + ID, id);
	eeprom_update_word((uint16_t *)offset + LENGTH, length);
	eeprom_update_block(buffer, (uint16_t *)(offset + DATA), length);
	return true;
}

bool check_existing_id(uint16_t id)
{
	uint16_t index = 0;
	while (index < 1024)
	{
		uint16_t magic_number;
		eeprom_read_block(&magic_number, (uint16_t *)index, 1);
		if ( & ~MASK_FREE) != MAGIC_NUMBER)
			return false;
		else if ()

	}
}

bool eepromalloc_write(uint16_t id, void *buffer, uint16_t length)
{
	if ((eeprom_read_byte(0) & ~MASK_FREE) != MAGIC_NUMBER)
		safe_eeprom_write(buffer, 0, length, id);
	else if (check_existing_id(id))
		return false;
	else

	return true;
}

bool eepromalloc_read(uint16_t id, void *buffer, uint16_t length)
{

}

bool eepromalloc_free(uint16_t id)
{

}

int main()
{
	uart_init();
	while(1)
	{
		char buffer[50] = {0};
		if (safe_eeprom_write("coucou comment vas tu ?\r\n", 0, 25) == false)
			uart_printstr("error 1\r\n");
		if (safe_eeprom_read(buffer, 0, 25) == false)
			uart_printstr("error\r\n");
		uart_printstr(buffer);

		bzero(buffer, 50);
		if (safe_eeprom_write("hey?\r\n", 26, 6) == false)
			uart_printstr("error 2\r\n");
		if (safe_eeprom_read(buffer, 26, 6) == false)
			uart_printstr("error 3\r\n");
		uart_printstr(buffer);

		if (safe_eeprom_read(buffer, 29, 6) == false)
			uart_printstr("error 4\r\n");

		_delay_ms(500);
		uart_printstr("Restart\r\n");
	}
}
