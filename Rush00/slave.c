#include "slave.h"
#include "uart.h"

void init_slave()
{
	TWAR = ADDRESS << 1;
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
}

uint8_t light_countdown_slave()
{
	uint8_t is_press = 0;
	uint8_t in_game = 0;

	while(1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			// uart_printstr("press\r\n");
			TWCR = (1 << TWEN) | (1 << TWINT);
			is_press = 1;
		}
		else
		{
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		}
		while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
		{}
		uint8_t c = TWDR;
		switch (c)
		{
			case LED_D1:
				// uart_printhex(c);
				// uart_printstr(" d1 ");
				TOGGLE(PORTB, PB0);
				break;
			case LED_D2:
				// uart_printhex(c);
				// uart_printstr(" d2 ");
				TOGGLE(PORTB, PB1);
				break;
			case LED_D3:
				// uart_printhex(c);
				// uart_printstr(" d3 ");
				TOGGLE(PORTB, PB2);
				break;
			case LED_D4:
				// uart_printhex(c);
				// uart_printstr(" d4 ");
				TOGGLE(PORTB, PB4);
				break;
			case GREEN_LED:
				// uart_printhex(c);
				// uart_printstr(" d5 ");
				in_game = 1;
				set_green_light();
				break;
			case WINNER:
				uart_printstr("winner\r\n");
				set_light();
				is_press = 1;
				in_game = 1;
				TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
				while (!(TWCR & (1 << TWINT)))
				{}
				break;
			default:
				break;
		}
		// uart_printhex(TW_STATUS);
		// uart_printstr("\r\n");
		if(is_press == 1 && in_game == 1)
			return WINNER;
		else if ((is_press == 1 && in_game == 0) || TW_STATUS == TW_SR_STOP)
			return LOSER;
	}
}

void slave_mode()
{
	uart_printstr("Start slave ...\r\n");
	uint8_t is_press = 0;
	while(1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			// uart_printstr("1send non ack\r\n");
			TWCR = (1 << TWEN) | (1 << TWINT);
			is_press = 1;
		}
		else
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
		{}
		unsigned char c = TWDR;
		// uart_printhex(TW_STATUS);
		if (is_press == 1)
			break;
	}
	set_blue_light();
	init_slave();
	while (!(TWCR & (1 << TWINT)) && TW_STATUS != TW_SR_SLA_ACK)
	{
		// uart_printhex(TW_STATUS);
	}
	_delay_ms(1000);
	uart_printstr("restart");
	end_game(light_countdown_slave());
	_delay_ms(5000);
}