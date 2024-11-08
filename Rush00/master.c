#include "master.h"

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
	// uart_printdec(max);
	while (cpt < max) {
		for (int i = 0; i < 4; i++) {
			for (volatile uint32_t i = 0; i < 45000; i++)
			{
				if ((PIND & (1 << PD2)) == 0)
				{
					set_light();
					return LOSER;
				}
			}
			if (i == 3)
				i++;
			uint8_t status = i2c_write(LED_D1 + i);
			// uart_printhex(status);
			if(status == TW_MT_DATA_NACK)
				return WINNER;
			TOGGLE(PORTB, PB0 + i);
		}
		cpt++;
	}
	if(i2c_write(GREEN_LED) == TW_MT_DATA_NACK)
		return WINNER;
	set_green_light();
	return CONTINUE;
}

uint8_t check_button()
{
	while(1)	//check press button of slave
	{
		uint8_t status = i2c_write(BUTTON_PRESS);
		uint8_t press = (PIND & (1 << PD2));
		// uart_printhex(status);
		// uart_tx(' ');
		// uart_printdec(press);
		// uart_printstr("\r\n");
		if (status == TW_MT_DATA_NACK && press != 0)
			return LOSER;
		else if (status == TW_MT_DATA_ACK && press == 0)
			return WINNER;
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
	set_blue_light();
	i2c_stop();
	_delay_ms(100);
	i2c_start(TW_WRITE, ADDRESS);
	_delay_ms(1000);
	uint8_t res = stair_leds();
	if (res == CONTINUE)
		res = check_button();
	else if (res == LOSER)
		i2c_write(WINNER);
	// uart_printstr("finish\r\n");
	i2c_stop();
	end_game(res);
	_delay_ms(5000);
}