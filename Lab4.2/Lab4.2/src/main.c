#include <stdio.h>#include "LED.h"#include "timer.h"

int main (void)
{
	// Init everything
	LED0_init();
	TC_init();
	TC_start();
	TC0_REG_RC = 10000;	// Set RC
	
	int pwm_low = 1;	// PWM status
	
	float dutyCycle = 0.9; //Sets the duty cycle
	
	while(1)
	{
		// When timer exceeds the duty threshold the PWM gets high
		if(get_tc_cv() > TC0_REG_RC*(1-dutyCycle))
		{
			pwm_low = 0;			// PWM is not low
			LED0_output(pwm_low);
		}
		else // When the timer is lower than the threshold the PWM is low
		{
			pwm_low = 1;			// PWM is low
			LED0_output(pwm_low);
		}
	}
}
