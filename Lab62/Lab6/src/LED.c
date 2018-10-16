#include "LED.h"

void initLED (void)
{
	/*
	* First make access to the GPIO port registers
	* shorter to type , just for our convenience
	*/
	volatile avr32_gpio_port_t * led0_port;
	led0_port = &AVR32_GPIO.port[LED0_PORT];
	/*
	* Choose GPIO peripheral function ,
	* by writing a 1 to gpers , GPIO Enable Register Set
	*/
	led0_port->gpers = LED0_BIT_VALUE;
	led0_port->gpers = LED1_BIT_VALUE;
	led0_port->gpers = LED2_BIT_VALUE;
	/*
	* Set the output value register to 1 in order to
	* turn the LED off (1= off , 0= on in this case ) ,
	* by writing a 1 to ovrs , Output Value Register Set
	*/
	led0_port->ovrs = LED0_BIT_VALUE;
	led0_port->ovrs = LED1_BIT_VALUE;
	led0_port->ovrs = LED2_BIT_VALUE;
	/*
	* Set the output enable register ,
	* by writing a 1 to oders , Output Drive Enable
	* Register Set
	*/
	led0_port->oders = LED0_BIT_VALUE;
	led0_port->oders = LED1_BIT_VALUE;
	led0_port->oders = LED2_BIT_VALUE;
}

void toggleLED(int ledNr)
{
	AVR32_GPIO.port[LED0_PORT].ovrt = ledNr;
}

void onLED(int ledNr)
{
	AVR32_GPIO.port[LED0_PORT].ovrc = ledNr;
}

void offLED(int ledNr)
{
	AVR32_GPIO.port[LED0_PORT].ovrs = ledNr;
}