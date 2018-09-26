#include <stdio.h>#include "LED.h"#include "timer.h"

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	LED0_init();
	TC_init();
	TC_start();
	TC0_REG_RC = 65000;
	
	int counter = 0;
	int pwm_low = 1;
	int counterMax = 10000;
	
	float dutyCycle = 0.9; //Sets the duty cycle
	
	
	while(1)
	{
		counter = get_tc_cv();
		
		if(counter > counterMax*(1-dutyCycle))
		{
			pwm_low = 0;
			LED0_output(pwm_low);
		}
		if(counter > counterMax)
		{
			counter = 0;
			TC_start();
			pwm_low = 1;
			LED0_output(pwm_low);
		}
		
		/*
		int i = 0;
		LED0_output(1);
		while(i < 10000)
		{
			i++;
			
		}
		i = 0;
		LED0_output(0);
		while(i < 1000)
		{
			i++;
			counter = TC0_REG_CV;
		}
		*/
	}

	/* Insert application code here, after the board has been initialized. */
}
