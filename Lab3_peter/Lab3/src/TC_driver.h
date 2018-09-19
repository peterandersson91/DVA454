#ifndef TC_driver_H_
#define TC_driver_H_

#include "evk1100.h"
#include "tc.h"
#include "intc.h"
#include "USART_driver.h"

#define TC_CHANNEL 0

static void tc_irq(void);
void tc_init(volatile avr32_tc_t *tc);

#endif /* TC_H_ */