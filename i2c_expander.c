#include "i2c_expander.h"

void i2c_exp_send_command(uint8_t address, uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t nb_arg)
{
	i2c_start(TW_WRITE, address);
	//send command
	i2c_write(command);
	//send arg
	i2c_write(arg1);
	if (nb_arg == 2)
		i2c_write(arg2);
	i2c_stop();
}

void i2c_exp_toggle_pin(uint8_t address, uint8_t command, uint8_t pin)
{
	uint8_t tmp = i2c_exp_read_value(address, command);
	i2c_start(TW_WRITE, address);
	//send command
	i2c_write(command);
	//send arg
	i2c_write(tmp ^ (1 << pin));
	i2c_stop();
}

uint8_t i2c_exp_read_value(uint8_t address, uint8_t command)
{
	i2c_start(TW_WRITE, address);
	//send command
	i2c_write(command);

	//restart in read mode
	i2c_start(TW_READ, address);
	//receive arg and send nack to stop after
	uint8_t res = i2c_read(NACK);
	i2c_stop();

	return res;
}

void i2c_exp_print_one_digit(uint8_t nb, uint8_t unity, uint8_t point)
{
	uint8_t tmp = i2c_exp_read_value(ADDRESS, 0x02);

	i2c_exp_send_command(ADDRESS, 0x02, tmp | 0xf0, 0, 2);
	tmp = (tmp & 0x0f) | (unity & 0xf0);
	switch(nb)
	{
		case 0:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, ZERO + (point << 7), 2);
			break;
		case 1:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, ONE + (point << 7), 2);
			break;
		case 2:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, TWO + (point << 7), 2);
			break;
		case 3:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, THREE + (point << 7), 2);
			break;
		case 4:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, FOUR + (point << 7), 2);
			break;
		case 5:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, FIVE + (point << 7), 2);
			break;
		case 6:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, SIX + (point << 7), 2);
			break;
		case 7:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, SEVEN + (point << 7), 2);
			break;
		case 8:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, HEIGHT + (point << 7), 2);
			break;
		case 9:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, NINE + (point << 7), 2);
			break;
		case '-':
			i2c_exp_send_command(ADDRESS, 0x02, tmp, TIRET + (point << 7), 2);
			break;
		default:
			i2c_exp_send_command(ADDRESS, 0x02, tmp, 0, 2);
	}
}

void i2c_exp_print_number(uint16_t nb)
{
	i2c_exp_print_one_digit(nb / 1000, DIGIT_1, 0);
	nb = nb % 1000;
	i2c_exp_print_one_digit(nb / 100, DIGIT_2, 0);
	nb = nb % 100;
	i2c_exp_print_one_digit(nb / 10, DIGIT_3, 0);
	nb = nb % 10;
	i2c_exp_print_one_digit(nb, DIGIT_4, 0);
}

void i2c_exp_print_float(float nb)
{
	uart_printdec(nb);
	uart_printstr("\r\n");
	if (nb < 0)
		i2c_exp_print_one_digit('-', DIGIT_1, 0);
	// else
	// i2c_exp_print_one_digit(nb / 10, DIGIT_2, 0);

	i2c_exp_print_one_digit(nb / 10, DIGIT_2, 0);
	i2c_exp_print_one_digit((uint32_t)nb % 10, DIGIT_3, 1);
	nb = (uint32_t)(nb * 10) % 10;
	i2c_exp_print_one_digit(nb , DIGIT_4, 0);
}