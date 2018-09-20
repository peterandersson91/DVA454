#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "USART_driver.h"
#include "stopwatch.h"

// Function to create a delay (not in milliseconds)
void mdelay(int milliseconds)
{
	while (milliseconds != 0)
	{
		milliseconds--;
	}
}

int main(void)
{
	volatile avr32_usart_t * usart = USART;
	volatile avr32_tc_t *tc = &AVR32_TC;
	USART_init(usart);
	tc_init(tc);
	stopwatch_init();
	
	
	struct time_struct ts;
	ts.hour = 0;
	ts.minute = 0;
	ts.second = 0;
	ts.centa = 0;
	
	while(1)
	{
		ts = update_time(centa_s);
		displayTime(ts);
		checkInput();
	}
	while(1);
}