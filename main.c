#include "tool.h"
#include "i2c.h"
#include "uart.h"

#define LED_PORT PORTB
#define LED_DDR DDRB
#define ADDRESS 0x18
#define BUTTON_PRESS 0x10
#define LED_D1 0x12
#define LED_D2 0x13
#define LED_D3 0x14
#define LED_D4 0x16
#define GREEN_LED 0x17
#define BLUE_LED 0x18
#define RED_LED 0x19
#define WINNER 1
#define LOSER 0
#define CONTINUE 2

void init_slave();
void slave_mode();
void master_mode();
void init_LED_SW1();
void init_timer();
void end_game(uint8_t res);
void set_light();

int main()
{
	init_LED_SW1();
	uart_init();
	i2c_init();
	init_slave();
	init_timer();
	while(1)
	{

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
			uart_printstr("press\r\n");
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
				uart_printhex(c);
				uart_printstr(" d1 ");
				TOGGLE(PORTB, PB0);
				break;
			case LED_D2:
				uart_printhex(c);
				uart_printstr(" d2 ");
				TOGGLE(PORTB, PB1);
				break;
			case LED_D3:
				uart_printhex(c);
				uart_printstr(" d3 ");
				TOGGLE(PORTB, PB2);
				break;
			case LED_D4:
				uart_printhex(c);
				uart_printstr(" d4 ");
				TOGGLE(PORTB, PB4);
				break;
			case GREEN_LED:
				uart_printhex(c);
				uart_printstr(" d5 ");
				in_game = 1;
				RESET(PORTD, PD3);
				RESET(PORTD, PD5);
				SET(PORTD, PD6);
				break;
			default:
				break;
		}
		uart_printhex(TW_STATUS);
		uart_printstr("\r\n");
		if ((is_press == 1 && in_game == 0) || TW_STATUS == TW_SR_STOP)
			return LOSER;
		else if(is_press == 1 && in_game == 1)
			return WINNER;
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
			uart_printstr("1send non ack\r\n");
			TWCR = (1 << TWEN) | (1 << TWINT);
			is_press = 1;
		}
		else
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT))) //wait for ack of the slave
		{}
		unsigned char c = TWDR;
		if (is_press == 1)
			break;
	}
	SET(PORTD, PD3);
	init_slave();
	while (!(TWCR & (1 << TWINT)) && TW_STATUS != TW_SR_SLA_ACK)
	{}
	_delay_ms(1000);
	uart_printstr("restart");
	end_game(light_countdown_slave());
	_delay_ms(5000);
}

void init_timer() {
	TCCR0B |= (1 << CS01); // Le timer que l'on utilisera est le Timer0
}

// Fonction pour obtenir un nombre entre 2 et 10
unsigned int get_random_number() {
	// Lecture de la valeur actuelle du Timer0
	unsigned int timerValue = TCNT0; // Entre 0 et 255
	// Etant donne qu'il compte tres rapidement (environ 16 000 000 de fois par seconde), on aura souvent des valeurs aleatoires
	// Calcul d'un nombre entre 2 et 10 à l'aide du modulo
	unsigned int randomNumber = (timerValue % 9) + 2; // % 9 donne un resultat entre 0 et 8, +2 pour obtenir entre 2 et 10
	return randomNumber;
}

uint8_t stair_leds() {
	uint8_t cpt = 0;
	uint8_t max = get_random_number();
	uart_printdec(max);
	while (cpt < max) {
		for (int i = 0; i < 4; i++) {
			for (volatile uint32_t i = 0; i < 45000; i++)
			{
				if ((PIND & (1 << PD2)) == 0)
					return LOSER;
			}
			// _delay_ms(100);
			if (i == 3)
				i++;
			uint8_t status = i2c_write(LED_D1 + i);
			uart_printhex(status);
			if(status == TW_MT_DATA_NACK)
				return WINNER;
			TOGGLE(PORTB, PB0 + i);
		}
		cpt++;
	}
	if(i2c_write(GREEN_LED) == TW_MT_DATA_NACK)
		return WINNER;
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	SET(PORTD, PD6);
	return CONTINUE;
}

uint8_t check_button()
{
	while(1)	//check press button of slave
	{
		uint8_t status = i2c_write(BUTTON_PRESS);
		uint8_t press = (PIND & (1 << PD2));
		uart_printhex(status);
		uart_tx(' ');
		uart_printdec(press);
		uart_printstr("\r\n");
		if (status == TW_MT_DATA_NACK && press != 0)
			return LOSER;
		else if (status == TW_MT_DATA_ACK && press == 0)
			return WINNER;
	}
}

void end_game(uint8_t res)
{
	if (res == WINNER)
	{
		uart_printstr("winner\r\n");
		SET(PORTD, PD3);
		RESET(PORTD, PD5);
		RESET(PORTD, PD6);
		_delay_ms(500);
		RESET(PORTD, PD3);
		RESET(PORTD, PD5);
		SET(PORTD, PD6);
	}
	if (res == LOSER)
	{
		uart_printstr("loser\r\n");
		SET(PORTD, PD3);
		RESET(PORTD, PD5);
		RESET(PORTD, PD6);
		_delay_ms(500);
		RESET(PORTD, PD3);
		SET(PORTD, PD5);
		RESET(PORTD, PD6);
	}
}

void master_mode()
{
	uart_printstr("Being matser ... \r\n");
	i2c_start(TW_WRITE, ADDRESS);
	while(1)	//check press button of slave
	{
		uint8_t status = i2c_write(BUTTON_PRESS);
		if (status == TW_MT_DATA_NACK)
			break ;
	}
	SET(PORTD, PD3);
	i2c_stop();
	_delay_ms(100);
	i2c_start(TW_WRITE, ADDRESS);
	_delay_ms(1000);
	uint8_t res = stair_leds();
	if (res == CONTINUE)
		res = check_button();
	uart_printstr("finish\r\n");
	i2c_stop();
	end_game(res);
	_delay_ms(5000);
}