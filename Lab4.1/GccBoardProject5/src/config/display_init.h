/********************************************************
 Name          : main.c
 Author        : Marcus Jansson
 Copyright     : GPL
 Description   : EVK1100 template
 **********************************************************/

#ifndef _DISPLAY_INIT_
#define _DISPLAY_INIT_

// Include Files
#include "board.h"
#include "compiler.h"
#include "adc.h"
#include "delay.h"
#include "dip204.h"
#include "gpio.h"
#include "spi.h"
#include "pm.h"

#define ON 1
#define OFF 0

void display_init(void);

#endif	//	_DISPLAY_INIT_
