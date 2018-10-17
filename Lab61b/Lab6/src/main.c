#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
<<<<<<< HEAD
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "string.h"#define DATALEN 10#define BUFFER_SIZE 10xSemaphoreHandle xFillCount;xSemaphoreHandle xEmptyCount;xQueueHandle xQHandle;xTaskHandle xHandleProducer;
xTaskHandle xHandleConsumer;//int byteCount = 0;struct msg{	int id;	char data[DATALEN];	portTickType timestamp;};
void mdelay(int msec)
{
	long volatile cycles = msec * 12000/11;
	while(cycles != 0)
	{
		cycles--;
	}
}
=======
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "string.h"#define DATALEN 10#define BUFFER_SIZE 10xSemaphoreHandle xSemaphore;	// Semaphore handle - QueuexQueueHandle xQHandle;			// Queue handle - Producer, ConsumerxTaskHandle xHandleProducer;	// Task handle - Producer
xTaskHandle xHandleConsumer;	// Task handle - Consumerint byteCount = 0;				// How long is the queuestruct msg{	int id;	char data[DATALEN];	portTickType timestamp;};
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59

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

void vProducer(void *pvParameters)
<<<<<<< HEAD
{	
	//portTickType xLastWakeTime;
	//const portTickType xFreq = TASK_DELAY_MS(1000);
	
	//xLastWakeTime = xTaskGetTickCount();
	
=======
{		
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
	struct msg message;
	message.id = 0;
	//message.data = 0;
	message.timestamp = 0;
	
	struct msg *pMesg = &message;
	
	char data[DATALEN] = "hej\n";
<<<<<<< HEAD
	//int i = byteCount;
=======
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
	
	onLED(LED0_BIT_VALUE);
	onLED(LED0_BIT_VALUE);		// On when the producer is active
	
	while(1)
	{
<<<<<<< HEAD
		offLED(LED0_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
=======
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
				vTaskSuspend(NULL);			// Go to sleep
				vTaskPrioritySet(NULL, 1);	// Back to normal priority
			}
			else if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore GIVEN\n");
			}
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
		}
		onLED(LED0_BIT_VALUE);
		
<<<<<<< HEAD
		strcpy(pMesg->data, data);
=======
		strcpy(pMesg->data, data);			// Copy data to pMesg->data
		
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
		xQueueSendToBack(xQHandle, &pMesg, (portTickType)10);
		
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
<<<<<<< HEAD
			// Semaphore given
=======
			//usart_write_line(serialPORT_USART,"PRODUCER2 Semaphore TAKEN\n");
			byteCount++;	// One more message in queue
			
			if(byteCount == 1)	// There are messages to read
			{
				vTaskResume(xHandleConsumer);	// Wake up consumer
				onLED(LED1_BIT_VALUE);			// On when the consumer is active
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"PRODUCER2 Semaphore GIVEN\n");
			}
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
		}
	}
}

void vConsumer( void *pvParameters )
<<<<<<< HEAD
{
	//portTickType xLastWakeTime;
	//const portTickType xFreq = TASK_DELAY_MS(10000);
	//volatile portTickType elapsed;
	
=======
{	
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
	struct msg *pMesg;
	
	onLED(LED1_BIT_VALUE);
	onLED(LED1_BIT_VALUE);	// On when the consumer is active
	
	while(1)
	{
		offLED(LED1_BIT_VALUE);
		if(xSemaphoreTake(xFillCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
<<<<<<< HEAD
			// Semaphore taken
=======
			//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore TAKEN\n");
			if(byteCount == 0)	// If the queue is empty
			{
				vTaskPrioritySet(NULL, 2);	// Raise priority to not be preempted before going to sleep
				offLED(LED1_BIT_VALUE);		// Off when the consumer is sleeping
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
				}
				vTaskSuspend(NULL);			// Go to sleep							
				vTaskPrioritySet(NULL, 1);	// Set priority back to normal
			}
			else if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
			}
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
		}
		onLED(LED1_BIT_VALUE);
		
		xQueueReceive(xQHandle, &pMesg, (portTickType)10);
<<<<<<< HEAD
		usart_write_line(serialPORT_USART, pMesg->data);
		
		if(xSemaphoreGive(xEmptyCount) == pdTRUE)
		{
			// Semaphore given
=======
		usart_write_line(serialPORT_USART, pMesg->data);		
		
		
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"CONSUMER2 Semaphore TAKEN\n");
			byteCount--;		// One message removed from queue
			
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
>>>>>>> a5aa0592f65820c5de2499cec45cf90cbc494c59
		}
	}
}

int main(void)
{
	initLED();
	init_usart();
	
	xFillCount = xSemaphoreCreateCounting(BUFFER_SIZE, 5);
	xEmptyCount = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
	
	xQHandle = xQueueCreate(10, sizeof(struct msg*));
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
	
	vTaskStartScheduler();
	
	for( ;; )
	{
		
	}
}

