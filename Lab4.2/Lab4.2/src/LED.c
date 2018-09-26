#include "LED.h"

void LED0_init(void)
{
	/* Select the pin we are going to use as GPIO */
	GPIO_PORT1_GPER |= (0x01 << 27);
	/* Enable the corresponding diver for that pin */
	GPIO_PORT1_ODER |= (0x01 << 27);
	/* Set the value of that pin to HIGH */
	GPIO_PORT1_OVR |= (0x01 << 27);
}void LED0_output(int value)
{
	if(value != 0)
	{
		/* Set the value on the LED0 pin to HIGH */
		GPIO_PORT1_OVR |= (0x01 << 27);
	}
	else
	{
		/* Set the value on the LED0 pin to LOW */
		GPIO_PORT1_OVR &= ~(0x01 << 27);
	}
}