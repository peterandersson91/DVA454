#ifndef TC_DRIVER_H_
#define TC_DRIVER_H_

#include "evk1100.h"
#include "tc.h"
#include "intc.h"
#include "USART_driver.h"

#define TC_CHANNEL 0

struct time_struct
{
	int hour;
	int minute;
	int second;
	int centa;
};

extern int centa_s;
extern bool update_timer;

static void tc_irq(void);
__attribute__((__interrupt__))static void tc_irq_handler(void);
void tc_init(volatile avr32_tc_t *tc);
struct time_struct update_time(int current_centa_s);

#endif /* TC_DRIVER_H_ */