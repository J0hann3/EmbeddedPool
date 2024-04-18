#include "tool.h"
#include "uart.h"
#define SPEED_INTERRUPT 15625

volatile static const char username[] = "jvigny";
volatile static const char password[] = "coucou";


void check_user(uint8_t *nb_error, uint8_t *index, unsigned char c)
{
	if (*nb_error != 0)
	{
		if (*nb_error == 255)
		{
			uart_printstr("Error: Username too long\r\n");
			RESET(UCSR0B, RXCIE0);
		}
		(*nb_error)++;
	}
	else if ((*index) < sizeof(username) / sizeof(char) && username[*index] == c)
		(*index)++;
	else
		(*nb_error)++;
	uart_tx(c);
}

void check_pass(uint8_t *nb_error, uint8_t *index, unsigned char c)
{
	if (*nb_error != 0)
	{
		if (*nb_error == 255)
		{
			uart_printstr("Error: Password too long\r\n");
			RESET(UCSR0B, RXCIE0);
		}
		(*nb_error)++;
	}
	else if ((*index) < sizeof(password) / sizeof(char) && password[*index] == c)
		(*index)++;
	else
		(*nb_error)++;
	uart_tx('*');
}

void delete_char(uint8_t *nb_error, uint8_t *index)
{
	if (*nb_error <= 0)
	{
		if (*index <= 0)
			return ;
		(*index)--;
	}
	else
		(*nb_error)--;
	uart_tx('\b');
	uart_tx(' ');
	uart_tx('\b');
}

void handle_enter(uint8_t *nb_error, uint8_t *index, uint8_t *check_username, uint8_t *read_password)
{
	uart_tx('\r');
		uart_tx('\n');
		if (*read_password == 0)
		{
			if (*nb_error == 0 && *index + 1 == sizeof(password) / sizeof(char))
				*check_username = 1;
			uart_printstr("	password:");
		}
		else if (*read_password == 1)
		{
			if (*nb_error == 0 && *check_username == 1 && *index + 1 == sizeof(password) / sizeof(char))
				uart_printstr("Welcome !\r\n");
			else
				uart_printstr("Bad combination/password\r\n");
			RESET(UCSR0B, RXCIE0);
		}
		*read_password = 1;
		*index = 0;
		*nb_error = 0;
}

ISR(USART_RX_vect)
{
	static uint8_t nb_error = 0;
	static uint8_t check_username = 0;
	static uint8_t read_password = 0;
	static uint8_t index = 0;

	unsigned char c = uart_rx();
	if (c == '\r')
		handle_enter(&nb_error, &index, &check_username, &read_password);
	else if (c == 127)
		delete_char(&nb_error, &index);
	else if (read_password == 0)
		check_user(&nb_error, &index, c);
	else if (read_password == 1)
		check_pass(&nb_error, &index, c);
}

int main()
{
	uart_init();
	if (sizeof(username) / sizeof(char) > 255)
	{
		uart_printstr("ERROR: Invalid size of username\r\n");
		RESET(UCSR0B, RXCIE0);
	}
	else if (sizeof(password) / sizeof(char) > 255)
	{
		uart_printstr("ERROR: Invalid size of password\r\n");
		RESET(UCSR0B, RXCIE0);
	}
	else
	{
		uart_printstr("Enter your login:\r\n");
		uart_printstr("	username:");
	}
	while (1)
	{
	}
}