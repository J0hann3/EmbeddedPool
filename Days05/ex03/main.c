#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <stdbool.h>
// #include <strings.h>

#define MAGIC_NUMBER 42
#define MASK_FREE 0b10000000
#define ID 1
#define LENGTH 3
#define DATA 5

bool safe_eeprom_read(void *buffer, size_t offset, size_t length)
{
	if (DATA + offset + length >= 1024)
		return false;
	if ((eeprom_read_byte((uint8_t *)offset) & ~MASK_FREE) != MAGIC_NUMBER)
		return false;
	uint16_t size = eeprom_read_word((void *)offset + LENGTH);
	if (size < length)
		length = size;
	eeprom_read_block(buffer, (uint16_t *)(offset + DATA), length);
	return true;
}

bool safe_eeprom_write(void *buffer, size_t offset, size_t length, uint16_t id)
{
	if (DATA + offset + length >= 1024)
		return false;
	eeprom_update_byte((void *)offset, MAGIC_NUMBER);
	eeprom_update_word((void *)offset + ID, id);
	eeprom_update_word((void *)offset + LENGTH, length);
	eeprom_update_block(buffer, (void *)(offset + DATA), length);
	return true;
}

bool check_existing_id(uint16_t id)
{
	size_t index = 0;
	while (index < 1024)
	{
		uint8_t magic_number = eeprom_read_byte((void *)index);
		if ((magic_number & ~MASK_FREE) != MAGIC_NUMBER)
			return false;
		if ((magic_number & MASK_FREE) == 0 && eeprom_read_word((void *)(index + ID)) == id)
			return true;
		index = index + DATA + eeprom_read_word((void *)(index + LENGTH));
	}
	return false;
}

size_t get_address_write(uint16_t length, bool *error)
{
	size_t index = 0;
	while (index < 1024)
	{
		uint8_t magic_number = eeprom_read_byte((void *)index);
		if ((magic_number & ~MASK_FREE) != MAGIC_NUMBER)
			return index;
		if ((magic_number & MASK_FREE) != 0 && eeprom_read_word((void *)(index + LENGTH)) == length)
			return index;
		index = index + DATA + eeprom_read_word((void *)(index + LENGTH));
	}
	*error = true;
	return index;
}

bool eepromalloc_write(uint16_t id, void *buffer, uint16_t length)
{
	if ((eeprom_read_byte(0) & ~MASK_FREE) != MAGIC_NUMBER)
	{
		if (!safe_eeprom_write(buffer, 0, length, id))
		{
			uart_printstr("Error : no space left to write buffer\r\n");
			return false;
		}
	}
	else if (check_existing_id(id))
	{
		uart_printstr("Error : id already existing\r\n");
		return false;
	}
	else
	{
		bool error = false;
		size_t address = get_address_write(length, &error);
		if (error == true)
		{
			uart_printstr("Error : no space left\r\n");
			return false;
		}
		if (!safe_eeprom_write(buffer, address, length, id))
		{
			uart_printstr("Error : no space left to write buffer\r\n");
			return false;
		}
	}
	return true;
}

size_t get_address_read(uint16_t id, bool *error)
{
	size_t index = 0;
	while (index < 1024)
	{
		uint8_t magic_number = eeprom_read_byte((void *)index);
		if ((magic_number & ~MASK_FREE) != MAGIC_NUMBER)
		{
			*error = true;
			return 0;
		}
		if ((magic_number & MASK_FREE) == 0 && eeprom_read_word((void *)(index + ID)) == id)
			return index;
		index = index + DATA + eeprom_read_word((void *)(index + LENGTH));
	}
	*error = true;
	return false;
}

bool eepromalloc_read(uint16_t id, void *buffer, uint16_t length)
{
	bool error = 0;
	size_t address = get_address_read(id, &error);
	if (error == 1)
	{
		uart_printstr("Error : no id find in memory\r\n");
		return false;
	}
	if (!safe_eeprom_read(buffer, address, length))
	{
		uart_printstr("Error : reading buffer\r\n");
		return false;
	}
	return true;
}

bool eepromalloc_free(uint16_t id)
{
	bool error = 0;
	size_t address = get_address_read(id, &error);
	if (error == 1)
	{
		uart_printstr("Error : no id find in memory\r\n");
		return false;
	}
	eeprom_update_byte((void *)address, MAGIC_NUMBER | MASK_FREE);
	return true;
}

int main()
{
	uart_init();
	while(1)
	{
		char buffer[50] = {0};
		if (eepromalloc_write(56, "coucou comment vas tu ?\r\n", 25) == false)
			uart_printstr("error 1\r\n");
		if (eepromalloc_read(56, buffer, 50) == false)
			uart_printstr("error\r\n");
		else
			uart_printstr(buffer);
		// eepromalloc_free(56);

		bzero(buffer, 50);
		if (eepromalloc_write(52, "hey?\r\n", 6) == false)
			uart_printstr("error 2\r\n");
		if (eepromalloc_read(52, buffer, 50) == false)
			uart_printstr("error 3\r\n");
		else
			uart_printstr(buffer);

		bzero(buffer, 50);
		if (eepromalloc_read(50, buffer, 50) == false)
			uart_printstr("error 4\r\n");
		else
			uart_printstr(buffer);
		// eepromalloc_free(52);

		_delay_ms(500);
		uart_printstr("Restart\r\n");
	}
}
