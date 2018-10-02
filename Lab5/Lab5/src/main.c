#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"

void vTask1(void *pvParameters)
{	
	portTickType xLastWakeTime;
	const portTickType xFreq = 10;
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED0_BIT_VALUE);
		
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vTask2( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = 20;
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED1_BIT_VALUE);
		
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vTask3( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = 30;
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		toggleLED(LED2_BIT_VALUE);
		
		vTaskDelayUntil(&xLastWakeTime, xFreq );
	}
}

void vTask4( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFreq = 100;
	
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			vTaskSuspend(pvParameters);
			onLED(LED0_BIT_VALUE);
			
			vTaskDelayUntil(&xLastWakeTime, xFreq );
			vTaskResume(pvParameters);
		}
		
	}
}

int main(void)
{
	initLED();

	static unsigned char ucParameterToPass ;
	xTaskHandle xHandle1;
	xTaskHandle xHandle2;
	xTaskHandle xHandle3;
	
		
	// Create the task , store the handle .
	xTaskCreate(	vTask1,
					"task1",
					configMINIMAL_STACK_SIZE,
					NULL,
					1,
					&xHandle1);
					
	xTaskCreate(	vTask2, "task2", configMINIMAL_STACK_SIZE, NULL, 1, &xHandle2);
	
	xTaskCreate(	vTask3, "task3", configMINIMAL_STACK_SIZE, NULL, 1, &xHandle3);
	
	xTaskCreate(	vTask4, "task4", configMINIMAL_STACK_SIZE, xHandle1, 1, NULL);
	
	vTaskStartScheduler();

	
	for( ;; )
	{
		
	}
}

