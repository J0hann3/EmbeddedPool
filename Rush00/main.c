#include "tool.h"
#include "slave.h"
#include "master.h"

void init_LED_SW1();
void init_timer();

int main()
{
	init_LED_SW1();
	uart_init();
	i2c_init();
	init_slave();
	init_timer();
	while(1)
	{
		// uart_printstr("wait state\r\n");
		if ((PIND & (1 << PD2)) == 0) //switch press == MASTER
		{
			master_mode();
			set_light();
			init_slave();
		}
		else if ((TWCR & (1 << TWINT)) && TW_STATUS == TW_SR_SLA_ACK) //SLAVE mode
		{
			slave_mode();
			set_light();
			init_slave();
		}
	}
}

void set_red_light()
{
	RESET(PORTD, PD3);
	SET(PORTD, PD5);
	RESET(PORTD, PD6);
}

void set_green_light()
{
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	SET(PORTD, PD6);
}

void set_blue_light()
{
	SET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);
}

void set_light()
{
	RESET(PORTB, PB0);
	RESET(PORTB, PB1);
	RESET(PORTB, PB2);
	RESET(PORTB, PB4);

	//
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);
}

void init_LED_SW1()
{
	RESET(DDRD, PD2);

	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);

	//
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);
	set_light();
}

void init_timer() {
	TCCR0B |= (1 << CS01); // Le timer que l'on utilisera est le Timer0
}

void end_game(uint8_t res)
{
	if (res == WINNER)
	{
		uart_printstr("winner\r\n");
		set_blue_light();
		_delay_ms(500);
		set_green_light();
	}
	if (res == LOSER)
	{
		uart_printstr("loser\r\n");
		set_blue_light();
		_delay_ms(500);
		set_red_light();
	}
}