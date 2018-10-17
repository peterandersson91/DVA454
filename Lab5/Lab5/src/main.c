#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"
// Initialize USART
void init_usart ( void )
{
	static const gpio_map_t USART_SERIAL_GPIO_MAP =
	{
		{ serialPORT_USART_RX_PIN , serialPORT_USART_RX_FUNCTION },
		{ serialPORT_USART_TX_PIN , serialPORT_USART_TX_FUNCTION }
	};
	static const gpio_map_t USART_DEBUG_GPIO_MAP =
	{
		{ configDBG_USART_RX_PIN , configDBG_USART_RX_FUNCTION } ,
		{ configDBG_USART_TX_PIN , configDBG_USART_TX_FUNCTION }
	};
	static const usart_options_t USART_OPTIONS =
	{
		. baudrate = 115200,
		. charlength = 8,
		. paritytype = USART_NO_PARITY ,
		. stopbits = USART_1_STOPBIT ,
		. channelmode = USART_NORMAL_CHMODE
	};
	pm_switch_to_osc0 (& AVR32_PM , FOSC0 , OSC0_STARTUP );
	gpio_enable_module ( USART_SERIAL_GPIO_MAP , 2);
	gpio_enable_module ( USART_DEBUG_GPIO_MAP , 2);
	usart_init_rs232 ( serialPORT_USART , & USART_OPTIONS , FOSC0 );
	usart_init_rs232 ( configDBG_USART , & USART_OPTIONS , FOSC0 );
}
// Led0 blinks every 1 sec
void vLED0Task(void *pvParameters)
{	
	portTickType xLastWakeTime;	// Holds tick count last led toggle
	const portTickType xFreq = TASK_DELAY_MS(1000);	// Holds the period
	
	xLastWakeTime = xTaskGetTickCount();	// Sets current tick count
	
	while(1)
	{
		toggleLED(LED0_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED0 TOGGLE\n");
		
		// Detects deadline misses
		while(xTaskGetTickCount() > xLastWakeTime + xFreq) // If current tick count exceeds the last wake time + period
		{
			usart_write_line (serialPORT_USART, "LED0 DEADLINE MISSED\n");
			xLastWakeTime += xFreq;	// Adds a period to the last wake time and rechecks if another deadline was missed
		}
		
		vTaskDelayUntil(&xLastWakeTime, xFreq ); // Sleep this task until it should toggle the led again
	}
}

// Led1 blinks every 2 sec
void vLED1Task( void *pvParameters )
{
	portTickType xLastWakeTime;	// Holds tick count last led toggle
	const portTickType xFreq = TASK_DELAY_MS(2000);	// Holds the period
	
	xLastWakeTime = xTaskGetTickCount(); // Sets current tick count
	
	while(1)
	{
		toggleLED(LED1_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED1 TOGGLE\n");
		
		// Detects deadline misses
		while(xTaskGetTickCount() > xLastWakeTime + xFreq) // If current tick count exceeds the last wake time + period
		{
			usart_write_line (serialPORT_USART, "LED1 DEADLINE MISSED\n");
			xLastWakeTime += xFreq; // Adds a period to the last wake time and rechecks if another deadline was missed
		}
		
		vTaskDelayUntil(&xLastWakeTime, xFreq ); // Sleep this task until it should toggle the led again
	}
}

// Led2 blinks every 3 sec
void vLED2Task( void *pvParameters )
{
	portTickType xLastWakeTime; // Holds tick count last led toggle
	const portTickType xFreq = TASK_DELAY_MS(3000); // Holds the period
	
	xLastWakeTime = xTaskGetTickCount(); // Sets current tick count
	
	while(1)
	{
		toggleLED(LED2_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED2 TOGGLE\n");
		
		// Detects deadline misses
		while(xTaskGetTickCount() > xLastWakeTime + xFreq) // If current tick count exceeds the last wake time + period
		{
			usart_write_line (serialPORT_USART, "LED2 DEADLINE MISSED\n");
			xLastWakeTime += xFreq; // Adds a period to the last wake time and rechecks if another deadline was missed
		}
		
		vTaskDelayUntil(&xLastWakeTime, xFreq ); // Sleep this task until it should toggle the led again
	}
}

// Lights the LED for 10 sec if the button is pressed 
void vButton0Task( void *pvParameters )
{
	portTickType xLastWakeTime; // Holds tick count before suspension
	const portTickType xFreq = TASK_DELAY_MS(10000); // Time to suspend the LED task
	
	while(1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();	// Sets current tick count
			vTaskSuspend(pvParameters);				// Suspends the corresponding LED task
			onLED(LED0_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 1 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq); // Delays this task until the suspension time is met
			vTaskResume(pvParameters);				// Resumes the LED task
		}
		
	}
}

// Lights the LED for 10 sec if the button is pressed 
void vButton1Task( void *pvParameters )
{
	portTickType xLastWakeTime; // Holds tick count before suspension
	const portTickType xFreq = TASK_DELAY_MS(10000); // Time to suspend the LED task
	
	while(1)
	{
		if(buttonIsPressed(BUTTON1_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();	// Sets current tick count
			vTaskSuspend(pvParameters);				// Suspends the corresponding LED task
			onLED(LED1_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 2 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq ); // Delays this task until the suspension time is met
			vTaskResume(pvParameters);				 // Resumes the LED task
		}
		
	}
}

// Lights the LED for 10 sec if the button is pressed 
void vButton2Task( void *pvParameters )
{
	portTickType xLastWakeTime; // Holds tick count before suspension
	const portTickType xFreq = TASK_DELAY_MS(10000); // Time to suspend the LED task
	
	while(1)
	{
		if(buttonIsPressed(BUTTON2_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();	// Sets current tick count
			vTaskSuspend(pvParameters);				// Suspends the corresponding LED task
			onLED(LED2_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 3 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq ); // Delays this task until the suspension time is met
			vTaskResume(pvParameters);				 // Resumes the LED task
		}
		
	}
}


int main(void)
{
	initLED();
	init_usart();

	xTaskHandle xHandle1;
	xTaskHandle xHandle2;
	xTaskHandle xHandle3;
	
		
	// Create the task, store the handle.
	xTaskCreate(	vLED0Task,
					"vLED0Task",
					configMINIMAL_STACK_SIZE,
					NULL,
					2,
					&xHandle1);
					
	xTaskCreate(	vLED1Task, "vLED1Task", configMINIMAL_STACK_SIZE, NULL, 2, &xHandle2);
	
	xTaskCreate(	vLED2Task, "vLED2Task", configMINIMAL_STACK_SIZE, NULL, 2, &xHandle3);
	
	xTaskCreate(	vButton0Task, "vButton0Task", configMINIMAL_STACK_SIZE, xHandle1, 1, NULL);
	
	xTaskCreate(	vButton1Task, "vButton1Task", configMINIMAL_STACK_SIZE, xHandle2, 1, NULL);
	
	xTaskCreate(	vButton2Task, "vButton2Task", configMINIMAL_STACK_SIZE, xHandle3, 1, NULL);
	
	vTaskStartScheduler();

	for( ;; )
	{
		
	}
}

