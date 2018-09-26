#include "LED.h"

void LED0_init(void)
{
	// Enables register for LED0
	GPIO_PORT1_GPER |= (0x01 << 27);
	// Makes the pin an output
	GPIO_PORT1_ODER |= (0x01 << 27);
	// Turns the led off
	GPIO_PORT1_OVR |= (0x01 << 27);
}void LED0_output(int value)
{
	if(value != 0)
	{
		// Turns the LED0 off
		GPIO_PORT1_OVR |= (0x01 << 27);
	}
	else
	{
		// Turns the LED0 on
		GPIO_PORT1_OVR &= ~(0x01 << 27);
	}
}