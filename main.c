#include "uart.h"
#include "i2c_expander.h"
#include "adc.h"
#include "spi.h"

volatile uint16_t sensor_counter = 0;
volatile uint16_t reset_sensor_counter = 0;
volatile uint16_t led_rgb_counter = 0;
volatile uint16_t reset_led_rgb_counter = 0;
volatile uint8_t get_date = 0;
volatile uint16_t counter_start = 0;

void init()
{
	spi_init();
	spi_set_led(0, 0, 0, 0, RESET_LED);
	spi_disable();
	uart_init();
	init_timer();
	i2c_init();

	//configure led D9, D10, D11 + 7-segments for output + sw3 input
	i2c_exp_send_command(ADDRESS, 0x06, 0b00000001, 0, 2);

	mode_6();
	//reset value to off
	i2c_exp_send_command(ADDRESS, 0x02, 0b11111111, 0xff, 2);


	//set led D1-D4 in output and rgb led
	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);
	SET(DDRD, PD3);
	SET(DDRD, PD5);
	SET(DDRD, PD6);

	// set light off for led D1-D4 and rgb led
	RESET(PORTB, PB0);
	RESET(PORTB, PB1);
	RESET(PORTB, PB2);
	RESET(PORTB, PB4);
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);

	//set SW1 SW2 in input
	RESET(DDRD, PD2);
	RESET(DDRD, PD4);
	sei();

}

uint8_t check_button_press(uint8_t button_state, int8_t *mode)
{
	// check button 1 press
	if ((button_state & (1 << SW1_STATE)) == 0 && (PIND & (1 << SW1)) == 0)
	{
		(*mode)++;
		if (*mode > MAX_MODE)
			*mode = MIN_MODE;
		i2c_exp_toggle_pin(ADDRESS, 0x02, D9);
		SET(button_state, SW1_STATE);
	}
	if ((button_state & (1 << SW1_STATE)) != 0 && (PIND & (1 << SW1)) != 0)
	{
		i2c_exp_toggle_pin(ADDRESS, 0x02, D9);
		RESET(button_state, SW1_STATE);
	}

	// check button 2 press
	if ((button_state & (1 << SW2_STATE)) == 0 && (PIND & (1 << SW2)) == 0)
	{
		(*mode)--;
		if (*mode < MIN_MODE)
			*mode = MAX_MODE;
		i2c_exp_toggle_pin(ADDRESS, 0x02, D10);
		SET(button_state, SW2_STATE);
	}
	if ((button_state & (1 << SW2_STATE)) != 0 && (PIND & (1 << SW2)) != 0)
	{
		i2c_exp_toggle_pin(ADDRESS, 0x02, D10);
		RESET(button_state, SW2_STATE);
	}

	// check button 3 press
	if ((button_state & (1 << SW3_STATE)) == 0 && (i2c_exp_read_value(ADDRESS, 0x00) & (1 << SW3)) == 0)
	{
		i2c_exp_toggle_pin(ADDRESS, 0x02, D11);
		SET(button_state, SW3_STATE);
	}
	if ((button_state & (1 << SW3_STATE)) != 0 && (i2c_exp_read_value(ADDRESS, 0x00) & (1 << SW3)) != 0)
	{
		i2c_exp_toggle_pin(ADDRESS, 0x02, D11);
		RESET(button_state, SW3_STATE);
	}
	return button_state;
}

void exec_mode(uint8_t mode)
{
	switch (mode)
	{
		case 0:
			mode_0();
			break;
		case 1:
			mode_1();
			break;
		case 2:
			mode_2();
			break;
		case 3:
			mode_3();
			break;
		case 4:
			mode_4();
			break;
		case 5:
			mode_5();
			break;
		case 6:
			mode_6();
			break;
		case 7:
			mode_7();
			break;
		case 8:
			mode_8();
			break;
		case 9:
			mode_9();
			break;
		case 10:
			mode_10();
			break;
		case 11:
			mode_11();
			break;
	}
}

void print_mode_binary(int8_t value)
{
	int res = PORTB & ~LIGHT_BITS;
	res |= (value & BIT_1) | (value & BIT_2) | (value & BIT_3) | ((value & BIT_4) << 1);
	PORTB = res;
}

void start_mode(uint8_t *start)
{
	if (counter_start > 4000 / 5)
		*start = 0;
	else if (counter_start > 3000 / 5)
	{
		i2c_exp_send_command(ADDRESS, 0x02, 0b11111111, 0xff, 2);
		RESET(PORTB, PB0);
		RESET(PORTB, PB1);
		RESET(PORTB, PB2);
		RESET(PORTB, PB4);
	}
	else
	{
		i2c_exp_print_number(8888);
		SET(PORTB, PB0);
		SET(PORTB, PB1);
		SET(PORTB, PB2);
		SET(PORTB, PB4);
	}
}

int main()
{
	uint8_t start = 1;
	uint8_t button_state = 0;
	int8_t mode = 0;
	init();

	while(1)
	{
		if (get_date == 1)
			update_date();
		button_state = check_button_press(button_state, &mode);
		if (start)
			start_mode(&start);
		else
		{
			print_mode_binary(mode);
			exec_mode(mode);
		}
	}
}
