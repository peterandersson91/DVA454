#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"xSemaphoreHandle xSemaphore;
void mdelay(int msec)
{
	long volatile cycles = msec * 12000/11;
	while(cycles != 0)
	{
		cycles--;
	}
}

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
		if(xSemaphoreTake(xSemaphore, (portTickType)10) == pdTRUE)
		{
			toggleLED(LED0_BIT_VALUE);
			usart_write_line (serialPORT_USART, "LED1 TOGGLE, SEMAPHORE TAKEN\n");
			
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				usart_write_line (serialPORT_USART, "LED 1, SEMAPHORE GIVEN\n");
			}
		}
		
		while(xTaskGetTickCount() > xLastWakeTime + xFreq)
		{
			usart_write_line (serialPORT_USART, "Priority inversion detected... LED0 DEADLINE MISSED\n");
			xLastWakeTime += xFreq;
		}
		//vTaskDelay(TASK_DELAY_MS(1000));
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vButton0Task( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(10000);
	volatile portTickType elapsed;
	
	while(1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			if(xSemaphoreTake(xSemaphore, (portTickType)10) == pdTRUE)
			{
				xLastWakeTime = xTaskGetTickCount();
				onLED(LED0_BIT_VALUE);
				usart_write_line (serialPORT_USART, "BUTTON 1 PRESSED, SEMAPHORE TAKEN, LED 1 on for 3sec\n");
			
				//vTaskDelayUntil(&xLastWakeTime, xFreq );
				
				mdelay(3000);
				
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					usart_write_line(serialPORT_USART, "BUTTON 1, SEMAPHORE GIVEN\n");
					elapsed = xTaskGetTickCount()-xLastWakeTime;
					int i = 0;
				}
			}
			else
			{
				usart_write_line (serialPORT_USART, "BUTTON 1 CANT TAKE SEMAPHORE\n");
			}
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
			
			onLED(LED1_BIT_VALUE);
			usart_write_line (serialPORT_USART, "BUTTON 2 PRESSED\n");
			
			while(buttonIsPressed(BUTTON1_PIN))
			{
				
			}
			
			toggleLED(LED1_BIT_VALUE);
		}
		else
		{
			vTaskDelay(TASK_DELAY_MS(10));
		}
		
	}
}

int main(void)
{
	initLED();
	init_usart();

	static unsigned char ucParameterToPass ;
	xTaskHandle xHandle1;
	
	vSemaphoreCreateBinary(xSemaphore);
	
		
	// Create the task , store the handle .
	xTaskCreate(	vLED0Task,
					"vLED0Task",
					configMINIMAL_STACK_SIZE,
					NULL,
					3,
					&xHandle1);
	
	xTaskCreate(	vButton0Task, "vButton0Task", configMINIMAL_STACK_SIZE, xHandle1, 1, NULL);
	
	xTaskCreate(	vButton1Task, "vButton1Task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	
	vTaskStartScheduler();

	
	for( ;; )
	{
		
	}
}

