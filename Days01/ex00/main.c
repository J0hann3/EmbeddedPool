#include <avr/io.h>
#include "tool.h"
#define NB_CYCLES 40


int main()
{
	unsigned long long cpt = 0;
	unsigned long long cpt_max = F_CPU / (2 * NB_CYCLES);

	SET(DDRB, PB1);

	while (1)
	{
		if (cpt >= cpt_max)
		{
			TOGGLE(PORTB, PB1);
			cpt = 0;
		}
		cpt++;
	}
}