#include "stopwatch.h"

bool timer_running = false;

void stopwatch_init(void)
{
	initLED();
	resetTimer();
}

void startTimer(void)
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrc = LED5_BIT_VALUE;
	led_port->ovrs = LED4_BIT_VALUE;
	Enable_global_interrupt();
	timer_running = true;
}

void stopTimer(void)
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrc = LED4_BIT_VALUE;
	led_port->ovrs = LED5_BIT_VALUE;
	Disable_global_interrupt();
	timer_running = false;
}

void resetTimer(void)
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrs = LED4_BIT_VALUE;
	led_port->ovrs = LED5_BIT_VALUE;
	centa_s = 0;
}

void checkInput(void)
{
	volatile avr32_usart_t * usart = USART;
	char input;
	if(usart->CSR.rxrdy != 0) // If there is something to read
	{
		input = USART_getChar();
		switch(input)
		{
			case 'a':
			startTimer();
			break;
			case 's':
			if (timer_running == true)
				stopTimer();
			break;
			case 'd':
			if (timer_running == false)
				resetTimer();
			break;
			default:
			break;
		}
	}
}

void displayTime(struct time_struct ts)
{	
	USART_putChar('\r');
	USART_put2Int(ts.hour);
	USART_putString(":");
	USART_put2Int(ts.minute);
	USART_putString(":");
	USART_put2Int(ts.second);
	USART_putString(":");
	USART_put2Int(ts.centa);
}

void initLED (void)
{
	// LED port pointer
	volatile avr32_gpio_port_t * led_port;
	// pointing to the correct port
	led_port = &AVR32_GPIO.port[LED_PORT];
	
	// Enables the registers for the LEDs
	led_port->gpers = LED0_BIT_VALUE;
	led_port->gpers = LED4_BIT_VALUE;
	led_port->gpers = LED5_BIT_VALUE;

	// Turning all LEDs off
	led_port->ovrs = LED0_BIT_VALUE;
	led_port->ovrs = LED4_BIT_VALUE;
	led_port->ovrs = LED5_BIT_VALUE;
	
	// setting the led pins to outputs
	led_port->oders = LED0_BIT_VALUE;
	led_port->oders = LED4_BIT_VALUE;
	led_port->oders = LED5_BIT_VALUE;
}