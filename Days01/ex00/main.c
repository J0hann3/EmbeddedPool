#include <avr/io.h>
#include "tool.h"
#define NB_CYCLES 24


int main()
{
	unsigned long cpt = 0;
	unsigned long cpt_max = F_CPU / NB_CYCLES;

	SET(DDRB, PB1);

	while (1)
	{
		if (cpt == cpt_max)
		{
			TOGGLE(PORTB, PB1);
			cpt = 0;
		}
		cpt++;
	}
}