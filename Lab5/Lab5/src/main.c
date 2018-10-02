#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"

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

void vLED0Task(void *pvParameters)
{	
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(1000);
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED0_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED0 TOGGLE\n");
		
		while(xTaskGetTickCount() > xLastWakeTime + xFreq)
		{
			usart_write_line (serialPORT_USART, "LED0 DEADLINE MISSED\n");
			xLastWakeTime += xFreq;
		}
		//vTaskDelay(TASK_DELAY_MS(1000));
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vLED1Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(2000);;
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED1_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED1 TOGGLE\n");
		
		while(xTaskGetTickCount() > xLastWakeTime + xFreq)
		{
			usart_write_line (serialPORT_USART, "LED1 DEADLINE MISSED\n");
			xLastWakeTime += xFreq;
		}
		//vTaskDelay(TASK_DELAY_MS(2000));
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vLED2Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(3000);
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED2_BIT_VALUE);
		usart_write_line (serialPORT_USART, "LED2 TOGGLE\n");
		
		while(xTaskGetTickCount() > xLastWakeTime + xFreq)
		{
			usart_write_line (serialPORT_USART, "LED2 DEADLINE MISSED\n");
			xLastWakeTime += xFreq;
		}
		//vTaskDelay(TASK_DELAY_MS(3000));
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vButton0Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(10000);
	
	while(1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();
			vTaskSuspend(pvParameters);
			onLED(LED0_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 1 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq );
			vTaskResume(pvParameters);
		}
		
	}
}

void vButton1Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(10000);
	
	while(1)
	{
		if(buttonIsPressed(BUTTON1_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();
			vTaskSuspend(pvParameters);
			onLED(LED1_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 2 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq );
			vTaskResume(pvParameters);
		}
		
	}
}

void vButton2Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(10000);
	
	while(1)
	{
		if(buttonIsPressed(BUTTON2_PIN))
		{
			xLastWakeTime = xTaskGetTickCount();
			vTaskSuspend(pvParameters);
			onLED(LED2_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 3 PRESSED\n");
			
			vTaskDelayUntil(&xLastWakeTime, xFreq );
			vTaskResume(pvParameters);
		}
		
	}
}


int main(void)
{
	initLED();
	init_usart();

	static unsigned char ucParameterToPass ;
	xTaskHandle xHandle1;
	xTaskHandle xHandle2;
	xTaskHandle xHandle3;
	
		
	// Create the task , store the handle .
	xTaskCreate(	vLED0Task,
					"vLED0Task",
					configMINIMAL_STACK_SIZE,
					NULL,
					1,
					&xHandle1);
					
	xTaskCreate(	vLED1Task, "vLED1Task", configMINIMAL_STACK_SIZE, NULL, 1, &xHandle2);
	
	xTaskCreate(	vLED2Task, "vLED2Task", configMINIMAL_STACK_SIZE, NULL, 1, &xHandle3);
	
	xTaskCreate(	vButton0Task, "vButton0Task", configMINIMAL_STACK_SIZE, xHandle1, 1, NULL);
	
	xTaskCreate(	vButton1Task, "vButton1Task", configMINIMAL_STACK_SIZE, xHandle2, 1, NULL);
	
	xTaskCreate(	vButton2Task, "vButton2Task", configMINIMAL_STACK_SIZE, xHandle3, 1, NULL);
	
	vTaskStartScheduler();

	
	for( ;; )
	{
		
	}
}

