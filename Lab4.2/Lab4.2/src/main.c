#include <stdio.h>

#define GPIO_BASE_ADDRESS 0xFFFF1000#define GPIO_PORT1_OFFSET 0x100

#define GPIO_PORT1_ADDRESS GPIO_BASE_ADDRESS+GPIO_PORT1_OFFSET

#define GPIO_GPER_OFFSET 0x00
#define GPIO_ODER_OFFSET 0x40
#define GPIO_OVR_OFFSET 0x50

#define GPIO_PORT1_GPER (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_GPER_OFFSET)))
#define GPIO_PORT1_ODER (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_ODER_OFFSET)))
#define GPIO_PORT1_OVR (*((volatile int*)(GPIO_PORT1_ADDRESS + GPIO_OVR_OFFSET)))// TIMER/COUNTER DEFINES#define TC_BASE_ADRESS	0xFFFF3800#define TC_CHANNEL0		0x00#define TC_CHANNEL0_ADRESS TC_BASE_ADRESS+TC_CHANNEL0#define TC_CCR			0x00#define TC_CMR			0x04#define TC_CV			0x10#define TC_RA			0x14#define TC_RB			0x18#define TC_RC			0x1C#define TC_SR			0x20#define TC0_REG_CMR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CMR)))
#define TC0_REG_CCR (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CCR)))
#define TC0_REG_CV (*((volatile int*)(TC_CHANNEL0_ADRESS + TC_CV)))

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

void TC_init()
{
	TC0_REG_CMR |= (0x01 << 1); // CLK selection, clock 3
	TC0_REG_CMR |= (0x01 << 14); // WAVESEL 10
	TC0_REG_CMR |= (0x01 << 15); // WAVEFORM MODE
}

void TC_start()
{
	TC0_REG_CCR |= (0x01 << 0); // Reset counter and the clock starts
	TC0_REG_CCR |= (0x01 << 2); // Enable clock
}


int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	LED0_init();
	TC_init();
	TC_start();
	int counter = 0;
	
	while(1)
	{
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
	}

	/* Insert application code here, after the board has been initialized. */
}
