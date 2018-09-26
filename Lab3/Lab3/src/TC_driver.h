#ifndef TC_DRIVER_H_
#define TC_DRIVER_H_

#include "evk1100.h"
#include "tc.h"
#include "intc.h"
#include "USART_driver.h"

#define TC_CHANNEL 0		// Selects Timer/Counter Channel

// Struct containing the time in hour, minute, seconds, hundredths
struct time_struct
{
	int hour;
	int minute;
	int second;
	int centa;
};

extern int centa_s;			// Global variable containing the time in hundredths

__attribute__((__interrupt__))static void tc_irq_handler(void); // Handle function for the RC interrupt
void tc_init(volatile avr32_tc_t *tc);							// Init the Timer/Counter
struct time_struct update_time(int current_centa_s);			// Updates the time to hours, minutes, seconds, hundredths from hundredths

#endif /* TC_DRIVER_H_ */