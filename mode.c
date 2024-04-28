#include "adc.h"
#include "i2c_expander.h"
#include "spi.h"

#define ADC8 0b1000

void mode_0()
{
	adc_10_AVCC_init();
	i2c_exp_print_number(adc_read(PC0));
}

void mode_1()
{
	adc_10_AVCC_init();
	i2c_exp_print_number(adc_read(PC1));
}

void mode_2()
{
	adc_10_AVCC_init();
	i2c_exp_print_number(adc_read(PC2));
}

void mode_3()
{
	adc_10_1V_init();
	i2c_exp_print_number(adc_read(ADC8) * 25. / 314);
}

void mode_4()
{
	static uint8_t next_color = RED | 0b10000000;

	spi_init();

	i2c_exp_print_one_digit('-', DIGIT_1, 0);
	i2c_exp_print_one_digit(4, DIGIT_2, 0);
	i2c_exp_print_one_digit(2, DIGIT_3, 0);
	i2c_exp_print_one_digit('-', DIGIT_4, 0);

	if ((next_color & 0b10000000) != 0 && reset_led_rgb_counter == 0)
	{
		next_color = GREEN;
		RESET(PORTD, GREEN_D5);
		RESET(PORTD, BLUE_D5);
		SET(PORTD, RED_D5);
		spi_set_led(0xff, 0, 0, 0x01, ALL_LED);
		reset_led_rgb_counter = 1;
	}
	if (led_rgb_counter > 1000 / 5 && reset_led_rgb_counter == 0)
	{
		if (next_color == RED)
		{
			RESET(PORTD, GREEN_D5);
			RESET(PORTD, BLUE_D5);
			SET(PORTD, RED_D5);
			spi_set_led(0xff, 0, 0, 0x01, ALL_LED);
		}
		if (next_color == GREEN)
		{
			RESET(PORTD, RED_D5);
			RESET(PORTD, BLUE_D5);
			SET(PORTD, GREEN_D5);
			spi_set_led(0, 0xff, 0, 0x01, ALL_LED);
		}
		if (next_color == BLUE)
		{
			RESET(PORTD, RED_D5);
			RESET(PORTD, GREEN_D5);
			SET(PORTD, BLUE_D5);
			spi_set_led(0, 0, 0xff, 0x01, ALL_LED);
		}
		next_color++;
		if (next_color > BLUE)
			next_color = RED;
		reset_led_rgb_counter = 1;
	}
	spi_disable();
}

void mode_5()
{
	uint8_t tmp = i2c_exp_read_value(ADDRESS, 0x02);
	i2c_exp_send_command(ADDRESS, 0x02, 0xf0 | tmp, 0xff, 2);
	spi_init();
	spi_set_led(0, 0, 0, 0, RESET_LED);
	spi_disable();
	RESET(PORTD, PD3);
	RESET(PORTD, PD5);
	RESET(PORTD, PD6);
}

static void get_sensor_measurement(float *temp, float *hum)
{
	static float last_temp = 0;
	static float last_hum = 0;
	static int8_t need_measurement = 1;

	if (need_measurement == 1)
	{
		i2c_start(TW_WRITE, 0x38);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		reset_sensor_counter = 1;
		need_measurement = 0;
	}
	if (sensor_counter > 80 / 5 && reset_sensor_counter == 0)
	{
		uint32_t temperature = 0;
		uint32_t humidity = 0;
		i2c_start(TW_READ, 0x38);
		i2c_read(ACK);
		humidity = i2c_read(ACK);
		humidity <<= 8;
		humidity |= i2c_read(ACK);
		humidity <<= 4;

		temperature = i2c_read(ACK);
		humidity |= (temperature & 0xf0) >> 4;

		temperature = temperature & 0x0f;
		temperature <<= 8;
		temperature |= i2c_read(ACK);
		temperature <<= 8;
		temperature |= i2c_read(NACK);
		i2c_stop();
		last_temp = temperature / 1048576. * 200 - 50;
		last_hum = humidity / 1048576. * 100;
		need_measurement = 1;
	}
	if (temp != 0)
		*temp = last_temp;
	if (hum != 0)
		*hum = last_hum;
}

void mode_6()
{
	float temp;
	get_sensor_measurement(&temp, 0);
	i2c_exp_print_float(temp);
}

void mode_7()
{
	float temp;
	get_sensor_measurement(&temp, 0);
	i2c_exp_print_float(temp * 1.8 + 32);
}

void mode_8()
{
	float hum;
	get_sensor_measurement(0, &hum);
	i2c_exp_print_float(hum);
}

void mode_9()
{
	i2c_start(TW_WRITE, 0b01010001);
	i2c_write(0x03);
	i2c_start(TW_READ, 0b01010001);

	uint8_t min = i2c_read(ACK);
	uint8_t hour = i2c_read(NACK);
	i2c_stop();

	i2c_exp_print_one_digit((hour & 0b00110000) >> 4, DIGIT_1, 0);
	i2c_exp_print_one_digit(hour & 0x0f, DIGIT_2, 1);

	i2c_exp_print_one_digit((min & 0b01110000) >> 4, DIGIT_3, 0);
	i2c_exp_print_one_digit(min & 0x0f, DIGIT_4, 1);
}

void mode_10()
{
	i2c_start(TW_WRITE, 0b01010001);
	i2c_write(0x05);
	i2c_start(TW_READ, 0b01010001);

	uint8_t day = i2c_read(ACK);
	i2c_read(ACK);
	uint8_t month = i2c_read(NACK);
	i2c_stop();

	i2c_exp_print_one_digit((day & 0b00110000) >> 4, DIGIT_1, 0);
	i2c_exp_print_one_digit(day & 0x0f, DIGIT_2, 0);

	i2c_exp_print_one_digit((month & 0b00010000) >> 4, DIGIT_3, 0);
	i2c_exp_print_one_digit(month & 0x0f, DIGIT_4, 1);

}

void mode_11()
{
	i2c_start(TW_WRITE, 0b01010001);
	i2c_write(0x07);
	i2c_start(TW_READ, 0b01010001);

	uint8_t century = i2c_read(ACK);
	uint8_t year = i2c_read(NACK);
	uint16_t tmp = ((year & 0xf0) >> 4) * 10;
	tmp += year & 0x0f;
	tmp += 1900;
	if ((century & 0x80) != 0)
		tmp += 100;

	i2c_stop();

	i2c_exp_print_number(tmp);
}
