#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAGIC_NUMBER 52
#define MASK_FREE 0b10000000
#define LENGTH 1
#define DATA 3

#define NONE 0
#define READ 1
#define WRITE 2
#define FORGET 3


bool safe_eeprom_read(void *buffer, size_t offset, size_t length)
{
	if (DATA + offset >= 1024)
		return false;
	if ((eeprom_read_byte((uint8_t *)offset) & ~MASK_FREE) != MAGIC_NUMBER)
		return false;
	uint16_t size = eeprom_read_word((void *)offset + LENGTH);
	if (size < length)
		length = size;
	if (DATA + offset + length >= 1024)
		return false;
	eeprom_read_block(buffer, (uint16_t *)(offset + DATA), length);
	return true;
}

bool safe_eeprom_write(void *buffer, size_t offset, size_t length)
{
	if (DATA + offset + length >= 1024)
		return false;
	eeprom_update_byte((void *)offset, MAGIC_NUMBER);
	eeprom_update_word((void *)offset + LENGTH, length);
	eeprom_update_block(buffer, (void *)(offset + DATA), length);
	return true;
}

bool check_existing_id(char *buffer, uint16_t length_key)
{
	size_t index = 0;
	while (index < 1024)
	{
		uint8_t magic_number = eeprom_read_byte((void *)index);
		if ((magic_number & ~MASK_FREE) != MAGIC_NUMBER)
			return false;
		if ((magic_number & MASK_FREE) == 0)
		{
			uint16_t i = 0;
			uint16_t length = eeprom_read_word((void *)(index + LENGTH));
			while(i < length)
			{
				if (i == length_key)
					return true;
				if (eeprom_read_byte((void *)(index + DATA + i)) != buffer[i])
					break;
				i++;
			}
		}
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

bool eepromalloc_write(void *buffer, uint16_t length, uint16_t length_key)
{
	if ((eeprom_read_byte(0) & ~MASK_FREE) != MAGIC_NUMBER)
	{
		if (!safe_eeprom_write(buffer, 0, length))
		{
			uart_printstr("\r\nError : no space left to write buffer\r\n");
			return false;
		}
	}
	else if (check_existing_id(buffer, length_key))
	{
		uart_printstr("\r\nError : id already existing\r\n");
		return false;
	}
	else
	{
		bool error = false;
		size_t address = get_address_write(length, &error);
		if (error == true)
		{
			uart_printstr("\r\nError : no space left\r\n");
			return false;
		}
		if (!safe_eeprom_write(buffer, address, length))
		{
			uart_printstr("\r\nError : no space left to write buffer\r\n");
			return false;
		}
	}
	return true;
}

size_t get_address_read(uint16_t length_key, bool *error, char *buffer)
{
	size_t index = 0;
	while (index < 1024)
	{
		uint8_t magic_number = eeprom_read_byte((void *)index);
		if ((magic_number & ~MASK_FREE) != MAGIC_NUMBER)
		{
			*error = true;
			return index;
		}
		if ((magic_number & MASK_FREE) == 0)
		{
			uint16_t i = 0;
			uint16_t length = eeprom_read_word((void *)(index + LENGTH));
			while(i < length)
			{
				if (i == length_key)
					return index;
				if (eeprom_read_byte((void *)(index + DATA + i)) != buffer[i])
					break;
				i++;
			}
		}
		index = index + DATA + eeprom_read_word((void *)(index + LENGTH));
	}
	*error = true;
	return index;
}

bool eepromalloc_read(void *buffer, uint16_t length, uint16_t length_key)
{
	bool error = 0;
	size_t address = get_address_read(length_key, &error, buffer);
	if (error == true)
	{
		uart_printstr("\r\nError : no id find in memory\r\n");
		return false;
	}
	for (uint16_t i = 0; i < 1024 - (DATA * 2); i++)
		((char *)buffer)[i] = 0;
	if (!safe_eeprom_read(buffer, address, length))
	{
		uart_printstr("\r\nError : reading buffer\r\n");
		return false;
	}
	return true;
}

bool eepromalloc_free(void *buffer, uint16_t length_key)
{
	bool error = 0;
	size_t address = get_address_read(length_key, &error, buffer);
	if (error == 1)
	{
		uart_printstr("\r\nError : no id find in memory\r\n");
		return false;
	}
	eeprom_update_byte((void *)address, MAGIC_NUMBER | MASK_FREE);
	return true;
}

static bool _skip_space(char *buffer, uint16_t length, uint16_t start)
{
	for (uint16_t i = start; i < length; i++)
	{
		if (buffer[i] != ' ')
			return false;
	}
	return true;
}

uint8_t get_command(char *buffer, uint16_t length)
{
	char *command[3] = {"READ", "WRITE", "FORGET"};

	for (uint8_t nb = 0; nb < 3; nb++)
	{
		for (uint16_t i = 0; i < length; i++)
		{
			if (command[nb][i] == '\0')
				return (_skip_space(buffer, length, i) ? nb + 1 : NONE);
			if (command[nb][i] != buffer[i])
				break ;
		}
	}
	return NONE;
}

uint8_t read_command(char *buffer)
{
	uint16_t index = 0;
	uint8_t command;
	uart_printstr("> ");

	while(1)
	{
		unsigned char c = uart_rx();
		uart_tx(c);
		buffer[index] = c;
		if (c == '"')
		{
			command = get_command(buffer, index);
			return command;
		}
		index++;
		if (index >= 1024 - (DATA * 2))
			return NONE;
	}
}

bool skip_space()
{
	unsigned char c;
	while(1)
	{
		c = uart_rx();
		uart_tx(c);
		if (c == '"')
		{
			return true;
		}
		else if (c != ' ')
			return false;
	}
}

bool write(char *buffer, uint16_t length)
{
	uint16_t length_key = 0;

	while(length_key < length)
	{
		if (buffer[length_key] == '"')
			break;
		length_key++;
	}
	if (length_key == 0)
	{
		uart_printstr("\r\nError: key word empty\r\n");
		return false;
	}
	length_key++;
	if (!eepromalloc_write(buffer, length, length_key))
	{
		return false;
	}
	uart_printstr("\r\nDone.\r\n");
	return true;
}

bool read(char *buffer, uint16_t length_key)
{
	if (length_key == 0)
	{
		uart_printstr("\r\nError: key word empty\r\n");
		return false;
	}
	bool status = eepromalloc_read(buffer, 1024 - (DATA * 2), length_key);
	if (status)
	{
		uart_printstr("\r\n");
		uart_printstr(buffer + length_key + 1);
		uart_printstr("\r\n");
	}
	return status;
}

bool freee(char *buffer, uint16_t length_key)
{
	if (length_key == 0)
	{
		uart_printstr("\r\nError: key word empty\r\n");
		return false;
	}
	bool status = eepromalloc_free(buffer, length_key);
	if (status)
		uart_printstr("\r\nDone.\r\n");
	return status;
}

bool read_key(char * buffer, uint8_t command)
{
	uint16_t index = 0;
	uint8_t args = 0;

	while(1)
	{
		unsigned char c = uart_rx();
		uart_tx(c);
		buffer[index] = c;
		if (c == '"')
		{
			if (command == READ)
				return (read(buffer, index));
			else if (command == WRITE)
			{
				if (args == 0)
				{
					args++;
					if (!skip_space())
					{
						uart_printstr("\r\nError\r\n");
						return false;
					}
					index++;
					continue ;
				}
				return (write(buffer, index));
			}
			else
				return (freee(buffer, index));
		}
		index++;
		if (index >= 1024 - (DATA * 2))
			return false;
	}

}

int main()
{
	uart_init();

	char *buffer = calloc((1024 - (DATA * 2)), sizeof(char));
	if (buffer == NULL)
	{
		uart_printstr("Error: malloc failed\r\n");
		return false;
	}
	uint8_t command;

	while(1)
	{
		command = read_command(buffer);
		if (command == NONE)
		{
			uart_printstr("\r\nError : wrong command\r\n");
			continue ;
		}
		read_key(buffer, command);
	}

	
}
