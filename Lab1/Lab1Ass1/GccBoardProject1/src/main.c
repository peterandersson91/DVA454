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
// This is the port which LED0_GPIO is located on
#define LED0_PORT (LED0_GPIO / GPIO_MAX_PIN_NUMBER)
// This is the bit position of the GPIO pin
#define LED0_PIN (LED0_GPIO & ( GPIO_MAX_PIN_NUMBER -1))
// This is a 1 - bit written to the bit position of the GPIO pin
#define LED0_BIT_VALUE (1 << LED0_PIN)

#define BUTTON_PORT (GPIO_PUSH_BUTTON_0 / GPIO_MAX_PIN_NUMBER)


#define BUTTON0_PIN (1 << (GPIO_PUSH_BUTTON_0 & (GPIO_MAX_PIN_NUMBER - 1)))

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
	volatile avr32_gpio_port_t * led0_port;
	led0_port = &AVR32_GPIO.port[LED0_PORT];
	/*
	* Choose GPIO peripheral function ,
	* by writing a 1 to gpers , GPIO Enable Register Set
	*/
	led0_port->gpers = LED0_BIT_VALUE;
	/*
	* Set the output value register to 1 in order to
	* turn the LED off (1= off , 0= on in this case ) ,
	* by writing a 1 to ovrs , Output Value Register Set
	*/
	led0_port->ovrs = LED0_BIT_VALUE;
	/*
	* Set the output enable register ,
	* by writing a 1 to oders , Output Drive Enable
	* Register Set
	*/
	led0_port->oders = LED0_BIT_VALUE;
}void initButton (void)
{

	volatile avr32_gpio_port_t * button_port;
	button_port = &AVR32_GPIO.port[BUTTON_PORT];

	button_port->gpers = BUTTON0_PIN;

	//button_port->ovrs = BUTTON0_BIT_VALUE;

	button_port->oderc = BUTTON0_PIN;
	//AVR32_GPIO.port[BUTTON_PORT].gpers = 1 << 24;
	//AVR32_GPIO.port[BUTTON_PORT].oderc = 1 << 24;
}void mdelay(int milliseconds){	while (milliseconds != 0){		milliseconds--;	}	}int main (void)
{
	initLED();
	initButton();
	int button0_state;
	int button_changed = 0;
	while(1){
		button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN);
	
		//int button_pressed = 0;
		/* Main loop that will toggle a single bit on the GPIO port
		*/
		if (button0_state == 0 && button_changed == 0){
			button_changed = 1;
			while(1) {
				AVR32_GPIO.port[LED0_PORT].ovrt = LED0_BIT_VALUE;
				mdelay(10000);
				button0_state = AVR32_GPIO.port[BUTTON_PORT].pvr & (BUTTON0_PIN);		
						
				if (button0_state != 0){
					button_changed = 0;
				}
				if (button0_state == 0 && button_changed == 0){
					button_changed = 1;
					AVR32_GPIO.port[LED0_PORT].ovrs = LED0_BIT_VALUE;
					break;
				}
			}
		}
		if(button0_state != 0)
			button_changed = 0;
	}
	while (1);
}