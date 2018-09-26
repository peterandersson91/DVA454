#include "stopwatch.h"

bool timer_running = false; // Global variable to see if the timer is running

// Initializing the stopwatch
void stopwatch_init(void)
{
	initLED();		// Init LED
	resetTimer();	// Resets the timer
}

// Enable interrupts to start the clock
void startTimer(void) 
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrc = LED5_BIT_VALUE;	// Lights the green led when the clock starts
	led_port->ovrs = LED4_BIT_VALUE;	// Turns red led off
	Enable_global_interrupt();			// Enables global interrupts to start the timer count
	timer_running = true;				// Timer is running
}

// Disable interrupts to stop the clock
void stopTimer(void)
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrc = LED4_BIT_VALUE;	// Green led off
	led_port->ovrs = LED5_BIT_VALUE;	// Red led on
	Disable_global_interrupt();			// Disables global interrupts to stop the timer count
	timer_running = false;				// Timer is stopped
}

// Resets global counting variable
void resetTimer(void)
{
	volatile avr32_gpio_port_t * led_port = &AVR32_GPIO.port[LED_PORT];
	led_port->ovrs = LED4_BIT_VALUE;	// Turn all leds off
	led_port->ovrs = LED5_BIT_VALUE;
	centa_s = 0;						// Resets counter
}

// Checks if there is something to read
void checkInput(void)
{
	volatile avr32_usart_t * usart = USART;
	char input;
	if(usart->CSR.rxrdy != 0) // If there is something to read
	{
		input = USART_getChar(); // gets the input
		switch(input)
		{
			case 'a':	// 'a' starts the timer
				startTimer();
			break;
			case 's':	// 's' stops the timer
			if (timer_running == true) // Stop only if timer is running
				stopTimer();
			break;
			case 'd':	// 'd' resets the timer
			if (timer_running == false) // Reset only if timer is stopped
				resetTimer();
			break;
			default:
			break;
		}
	}
}

// Writes the current time to USART
void displayTime(struct time_struct ts)
{	
	USART_putChar('\r');
	USART_put2Int(ts.hour); // hours
	USART_putString(":");
	USART_put2Int(ts.minute); // minutes
	USART_putString(":");
	USART_put2Int(ts.second); // seconds
	USART_putString(":");
	USART_put2Int(ts.centa); // hundredths
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