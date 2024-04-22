#include "tool.h"
#include "uart.h"
#include <avr/eeprom.h>
#include <stdbool.h>
// #include <strings.h>

#define MAGIC_NUMBER 42

bool safe_eeprom_read(void *buffer, size_t offset, size_t length)
{
	if (1 + offset + length >= 1024)
		return false;
	if (eeprom_read_byte((uint8_t *)offset) != MAGIC_NUMBER)
		return false;
	eeprom_read_block(buffer, (uint16_t *)(offset + 1), length);
	return true;
}

bool safe_eeprom_write(void *buffer, size_t offset, size_t length)
{
	if (1 + offset + length >= 1024)
		return false;
	eeprom_update_byte((uint8_t *)offset, MAGIC_NUMBER);
	eeprom_update_block(buffer, (uint16_t *)(offset + 1), length);
	return true;
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
