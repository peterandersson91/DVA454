#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "USART_driver.h"
#include "stopwatch.h"

int main(void)
{
	volatile avr32_usart_t * usart = USART;
	volatile avr32_tc_t *tc = &AVR32_TC;
	USART_init(usart);						// Init USART
	tc_init(tc);							// Init Timer/Counter
	stopwatch_init();						// Init Stopwatch
	
	// Struct containing the time in hours, minutes, seconds, hundredths
	struct time_struct ts;
	ts.hour = 0;
	ts.minute = 0;
	ts.second = 0;
	ts.centa = 0;
	
	while(1)
	{
		ts = update_time(centa_s);	// Updates the time struct with the current timer count
		displayTime(ts);			// Displays the time on the serial port
		checkInput();				// Checks input from serial port
	}
	while(1);
}