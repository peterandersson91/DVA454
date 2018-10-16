#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
#include "gpio.h"#include "pm.h"#include "semphr.h"#include "queue.h"#include "display_init.h"#include "string.h"#define DATALEN 10#define BUFFER_SIZE 10xSemaphoreHandle xSemaphore;xSemaphoreHandle xLCDSemaphore;xQueueHandle xQHandle;xTaskHandle xHandleProducer;
xTaskHandle xHandleConsumer;xTaskHandle xHandleStatus;int byteCount = 0;int statusTaskActive = pdFALSE;int statusCount = 0;struct msg{	int id;	char data[DATALEN];	portTickType timestamp;};
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

void vProducer(void *pvParameters)
{	
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(1000);
	
	xLastWakeTime = xTaskGetTickCount();
	
	struct msg message;
	message.id = 0;
	//message.data = 0;
	message.timestamp = 0;
	
	struct msg *pMesg = &message;
	
	char data[DATALEN] = "hej\n";
	int i = byteCount;
	
	char c;
	
	onLED(LED0_BIT_VALUE);
	
	while(1)
	{
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore TAKEN\n");
			if(byteCount == BUFFER_SIZE)
			{
				vTaskPrioritySet(NULL, 2);
				
				offLED(LED0_BIT_VALUE);
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore GIVEN\n");
				}
				vTaskSuspend(NULL);
				vTaskPrioritySet(NULL, 1);
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"PRODUCER1 Semaphore GIVEN\n");
			}
		}
		
		c = usart_getchar(serialPORT_USART);
		//c = 'b';
		
		//strcpy(pMesg->data, data);
		
		xQueueSendToBack(xQHandle, &c, (portTickType)10);
		
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"PRODUCER2 Semaphore TAKEN\n");
			byteCount++;
			
			if(byteCount == 1)
			{
				vTaskResume(xHandleConsumer);
				onLED(LED1_BIT_VALUE);
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
	portTickType xLastWakeTime;
	const portTickType xFreq = TASK_DELAY_MS(10000);
	volatile portTickType elapsed;
	
	struct msg *pMesg;
	
	char c;
	int column = 0;
	
	onLED(LED1_BIT_VALUE);
	
	while(1)
	{
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore TAKEN\n");
			if(byteCount == 0)
			{
				vTaskPrioritySet(NULL, 2);
				
				offLED(LED1_BIT_VALUE);
				if(xSemaphoreGive(xSemaphore) == pdTRUE)
				{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
				}
				vTaskSuspend(NULL);
							
				vTaskPrioritySet(NULL, 1);
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
					//Successfully given back
					//usart_write_line(serialPORT_USART,"CONSUMER1 Semaphore GIVEN\n");
			}
		}
			
		xQueueReceive(xQHandle, &c, (portTickType)10);
		statusCount++;
		
		//usart_write_line(serialPORT_USART, pMesg->data);
		
		column++;
		if(column > 20)
		{	
			dip204_clear_display();
			column = 1;		
		}
		if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			dip204_set_cursor_position(column, 1);	
			dip204_printf_string("%c", c); //print value
		
			if(statusTaskActive == pdTRUE)
			{
				dip204_set_cursor_position(1, 2);
				dip204_printf_string("%d", statusCount); //print value
			}
			if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
			{
				//Successfully given back
			}
		}
			
		if(xSemaphoreTake(xSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			//usart_write_line(serialPORT_USART,"CONSUMER2 Semaphore TAKEN\n");
			byteCount--;
			
			if(byteCount == BUFFER_SIZE - 1)
			{
				vTaskResume(xHandleProducer);
				onLED(LED0_BIT_VALUE);
			}
			if(xSemaphoreGive(xSemaphore) == pdTRUE)
			{
				//Successfully given back
				//usart_write_line(serialPORT_USART,"CONSUMER2 Semaphore TAKEN\n");
			}
		}
	}
}

void vStatus( void *pvParameters )
{
	while (1)
	{
		if(buttonIsPressed(BUTTON0_PIN))
		{
			statusTaskActive = pdTRUE;
			onLED(LED2_BIT_VALUE);
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
			{
				dip204_set_cursor_position(1, 2);	
				dip204_printf_string("%d", statusCount); //print value
				if(xSemaphoreGive(xLCDSemaphore) == pdTRUE)
				{
					//Successfully given back
				}
			}
			vTaskDelay(TASK_DELAY_MS(10000));//Sleep for 10 sec
			offLED(LED2_BIT_VALUE);
			statusTaskActive = pdFALSE;
			if(xSemaphoreTake(xLCDSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)
			{
				dip204_set_cursor_position(1, 2);	
				dip204_write_string("                    "); 
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

	static unsigned char ucParameterToPass ;
	
	vSemaphoreCreateBinary(xSemaphore);
	vSemaphoreCreateBinary(xLCDSemaphore);
	
	xQHandle = xQueueCreate(10, sizeof(char*));
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
	
	vTaskStartScheduler();

	
	for( ;; )
	{
		
	}
}

