# Rush01

main
	init
	while(1)
		if (SW1 press)
			switch on led D9
			mode++;
			print mode on D1-D4
		if (SW2 press)
			switch on led D10
			mode--;
			print mode on D1-D4
		if (SW3 press)
			switch on led D11
		print mode on segment


interrupt uart receive to read date