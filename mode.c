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
	i2c_exp_print_one_digit('-', DIGIT_1, 0);
	i2c_exp_print_one_digit(4, DIGIT_2, 0);
	i2c_exp_print_one_digit(2, DIGIT_3, 0);
	i2c_exp_print_one_digit('-', DIGIT_4, 0);

	//TODO flashes led D5, D6, D7, D8 every second
}

void mode_5()
{
	uint8_t tmp = i2c_exp_read_value(ADDRESS, 0x02);
	i2c_exp_send_command(ADDRESS, 0x02, 0xf0 | tmp, 0xff, 2);
	spi_set_led(0, 0, 0, 0, RESET_LED);
}

static void get_sensor_measurement(float *temp, float *hum)
{
	static float last_temp = 0;
	static float last_hum = 0;
	static int16_t last_time_measurement = -1;

	if (last_time_measurement == -1)
	{
		i2c_start(TW_WRITE, 0x38);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		counter = 0;
		last_time_measurement = 0;
	}
	else if (counter > last_time_measurement + 4)
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
		last_time_measurement = -1;
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