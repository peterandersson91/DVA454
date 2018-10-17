#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#define BUFFER_SIZE 10xSemaphoreHandle xSemaphore;		// Semaphore handle - QueuexSemaphoreHandle xLCDSemaphore;		// Semaphore handle - DisplayxQueueHandle xQHandle;				// Queue handle - Producer, ConsumerxTaskHandle xHandleProducer;		// Task handle - Producer
xTaskHandle xHandleConsumer;		// Task handle - ConsumerxTaskHandle xHandleStatus;			// Task handle - Statusint byteCount = 0;					// How long is the queueint statusTaskActive = pdFALSE;		// Show status on display?int statusCount = 0;				// How many characters have been sent

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
	pm_switch_to_osc0 (& AVR32_PM , FOSC0 , OSC0_STARTUP );				// Switch to 12 MHz clock
	gpio_enable_module ( USART_SERIAL_GPIO_MAP , 2);					// Enable usart
	gpio_enable_module ( USART_DEBUG_GPIO_MAP , 2);
	usart_init_rs232 ( serialPORT_USART , & USART_OPTIONS , FOSC0 );	// Usart options
	usart_init_rs232 ( configDBG_USART , & USART_OPTIONS , FOSC0 );
}

void vProducer(void *pvParameters)
{	
	char char_to_send;		// Character to send to Consumer
	
	onLED(LED0_BIT_VALUE);	// On when the producer is active
	
	while(1)
	{
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Waits for the queue semaphore
		{
			//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore TAKEN\n");
			if(byteCount == BUFFER_SIZE)	// If the message queue is full
			{				
				vTaskPrioritySet(NULL, 2);	// Raise priority to not be preempted before going to sleep			
				offLED(LED0_BIT_VALUE);		// Off when the producer is sleeping
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore GIVEN\n");
				}
				vTaskSuspend(NULL);			// Sleep
				vTaskPrioritySet(NULL, 1);	// Back to normal priority
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore GIVEN\n");
			}
		}
		
		char_to_send = usart_getchar(serialPORT_USART);		
		xQueueSendToBack(xQHandle, &char_to_send, (portTickType)10);
		
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"PRODUCER2 Semaphore TAKEN\n");
			byteCount++; // One more message in queue
			
			if(byteCount == 1)	// There are messages to read
			{
				vTaskResume(xHandleConsumer);	// Wake up Consumer
				onLED(LED1_BIT_VALUE);			// On when Consumer is active
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"PRODUCER2 Semaphore GIVEN\n");
			}
		}
		
	}
}

void vConsumer( void *pvParameters )
{
	volatile char char_received;		// Character received
	volatile int column = 0;			// Where on display to write
	
	onLED(LED1_BIT_VALUE);	// On when Consumer is active
	
	while(1)
	{
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore TAKEN\n");
			if(byteCount == 0)	// If the queue is empty
			{
				vTaskPrioritySet(NULL, 2);		// Raise priority to not be preempted before going to sleep					
				offLED(LED1_BIT_VALUE);			// Off when Consumer is sleeping
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
				}
				vTaskSuspend(NULL);				// Go to sleep							
				vTaskPrioritySet(NULL, 1);		// Set priority back to normal
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
			}
		}
			
		xQueueReceive(xQHandle, &char_received, (portTickType)10);
		statusCount++;		// One more character received
		
		//usart_write_line(serialPORT_USART, pMesg->data);
		
		column++;
		if(column > 20)		// If display is full, restart
		{	
			dip204_clear_display();
			column = 1;		
		}
		if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Semaphore - Display
		{
			dip204_set_cursor_position(column, 1);	
			dip204_printf_string("%c", char_received); //print value
		
			if(statusTaskActive == pdTRUE)		// Show status on display?
			{
				dip204_set_cursor_position(1, 2);
				dip204_printf_string("%d", statusCount); //print value
			}
			if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
			{
				//Successfully given back
			}
		}
			
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)		// Semaphore - Queue
		{
			//usart_write_line(serialPORT_USART,"CONSUMER2 Semaphore TAKEN\n");
			byteCount--;	// One message removed from queue
			
			if(byteCount == BUFFER_SIZE - 1)		// If there is place in queue
			{
				vTaskResume(xHandleProducer);		// Wake up Producer
				onLED(LED0_BIT_VALUE);				// On when producer is active
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"CONSUMER2 Semaphore TAKEN\n");
			}
		}
	}
}

// Shows total amount of characters sent
void vStatus( void *pvParameters )
{
	while (1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			statusTaskActive = pdTRUE;	// Show status on display
			onLED(LED2_BIT_VALUE);		// On when status is active
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)	// Semaphore - Display
			{
				dip204_set_cursor_position(1, 2);	
				dip204_printf_string("%d", statusCount); //print value
				if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
				{
					//Successfully given back
				}
			}
			vTaskDelay(TASK_DELAY_MS(10000));	// Sleep for 10 sec
			offLED(LED2_BIT_VALUE);				// Off when status is sleeping
			statusTaskActive = pdFALSE;			// Do not show status on display
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
			{
				dip204_set_cursor_position(1, 2);	
				dip204_write_string("                    ");	// Clear the status count row
				if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
				{
					//Successfully given back
				}
			}
		}
	}
}

int main(void)
{
	initLED();
	init_usart();
	display_init();
	dip204_clear_display();
	
	vSemaphoreCreateBinary(xSemaphore);		// Semaphore - Queue
	vSemaphoreCreateBinary(xLCDSemaphore);	// Semaphore - Display
	
	xQHandle = xQueueCreate(10, sizeof(char*));		// Queue - 10 spots of char-pointer size
	if(xQHandle == NULL)
	{
		//ERROR!
	}
	
	
	// Create the task , store the handle .
	xTaskCreate(	vProducer,
					"vProducer",
					configMINIMAL_STACK_SIZE,
					NULL,
					1,
					&xHandleProducer);
	xTaskCreate(	vConsumer, "vConsumer", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleConsumer);
	
	xTaskCreate(	vStatus, "vStatus", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleStatus);
	
	vTaskStartScheduler();		// Starts the schedueling

	
	for( ;; )
	{
		
	}
}

