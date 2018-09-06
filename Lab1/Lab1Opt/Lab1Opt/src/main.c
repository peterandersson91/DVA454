/****************************************************
* Name : main . c
* Author : Marcus Jansson
* Copyright : Not really
* Description : EVK1100 Hello LED
*****************************************************/
/* Include Files */
#include "board.h"
#include "compiler.h"
#include <unistd.h>

/* Define the ports and pins to be used */
// The maximum number of pins in a port
#define GPIO_MAX_PIN_NUMBER 32
// This is the port which all LED_GPIO's are located on
#define LED_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// This is the bit position of the GPIO pin
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED1_PIN (LED1_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED2_PIN (LED2_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED3_PIN (LED3_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED4_PIN (LED4_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
#define LED5_PIN (LED6_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// This is a 1 - bit written to the bit position of the GPIO pin
#define LED0_BIT_VALUE (1 << LED0_PIN)
#define LED1_BIT_VALUE (1 << LED1_PIN)
#define LED2_BIT_VALUE (1 << LED2_PIN)
#define LED3_BIT_VALUE (1 << LED3_PIN)
#define LED4_BIT_VALUE (1 << LED4_PIN)
#define LED5_BIT_VALUE (1 << LED5_PIN)

#define BUTTON_PORT (GPIO_PUSH_BUTTON_2 / GPIO_MAX_PIN_NUMBER)

#define BUTTON2_PIN (1 << (GPIO_PUSH_BUTTON_2 & (GPIO_MAX_PIN_NUMBER - 1)))

/*
* Init a GPIO pin for driving a LED
* Purpose : Set up the LED GPIO line as output and at
* value 1 which , contra intuitive , will turn the LED off .
*/
void initLED (void)
{
	/*
	* First make access to the GPIO port registers
	* shorter to type , just for our convenience
	*/
	volatile avr32_gpio_port_t * led_port;
	led_port = &AVR32_GPIO.port[LED_PORT];
	/*
	* Choose GPIO peripheral function ,
	* by writing a 1 to gpers , GPIO Enable Register Set
	*/
	led_port->gpers = LED0_BIT_VALUE;
	led_port->gpers = LED1_BIT_VALUE;
	led_port->gpers = LED2_BIT_VALUE;
	led_port->gpers = LED3_BIT_VALUE;
	led_port->gpers = LED4_BIT_VALUE;
	led_port->gpers = LED5_BIT_VALUE;
	/*
	* Set the output value register to 1 in order to
	* turn the LED off (1= off , 0= on in this case ) ,
	* by writing a 1 to ovrs , Output Value Register Set
	*/
	led_port->ovrs = LED0_BIT_VALUE;
	led_port->ovrs = LED1_BIT_VALUE;
	led_port->ovrs = LED2_BIT_VALUE;
	led_port->ovrs = LED3_BIT_VALUE;
	led_port->ovrs = LED4_BIT_VALUE;
	led_port->ovrs = LED5_BIT_VALUE;
	/*
	* Set the output enable register ,
	* by writing a 1 to oders , Output Drive Enable
	* Register Set
	*/
	led_port->oders = LED0_BIT_VALUE;
	led_port->oders = LED1_BIT_VALUE;
	led_port->oders = LED2_BIT_VALUE;
	led_port->oders = LED3_BIT_VALUE;
	led_port->oders = LED4_BIT_VALUE;
	led_port->oders = LED5_BIT_VALUE;
}void initButton (void)
{

	volatile avr32_gpio_port_t * button_port;
	button_port = &AVR32_GPIO.port[BUTTON_PORT];
	
	button_port->gpers = BUTTON2_PIN;
	button_port->oderc = BUTTON2_PIN;
	
}void mdelay(int milliseconds){	while (milliseconds != 0)	{		milliseconds--;	}	}
int main (void)
{
	initLED();
	initButton();
	int button2_state;
	int button_changed = 0;
	int led_pos = 0;
	int led_dir = 1;
	while(1)
	{
		switch(led_pos)
		{
			case 0:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED1_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED0_BIT_VALUE;
				led_dir = 1;
				break;				
			}
			case 1:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED2_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED1_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED0_BIT_VALUE;		
				break;
			}			
			case 2:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED3_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED2_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED1_BIT_VALUE;
				break;				
			}
			case 3:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED4_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED3_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED2_BIT_VALUE;
				break;				
			}
			case 4:
			{
				AVR32_GPIO.port[LED_PORT].ovrs = LED5_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrc = LED4_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED3_BIT_VALUE;
				break;				
			}
			case 5:
			{
				AVR32_GPIO.port[LED_PORT].ovrc = LED5_BIT_VALUE;
				AVR32_GPIO.port[LED_PORT].ovrs = LED4_BIT_VALUE;
				led_dir = 0;
				break;				
			}	
		}
		mdelay(1000);
		if(led_dir == 1)
			led_pos++;
		else
			led_pos--;
	}
	while(1);
}
