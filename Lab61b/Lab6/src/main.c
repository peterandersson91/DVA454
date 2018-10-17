#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#include "BUTTONS.h"
#include "usart.h"
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
	struct msg message;
	message.id = 0;
	//message.data = 0;
	message.timestamp = 0;
	
	struct msg *pMesg = &message;
	
	char data[DATALEN] = "hej\n";
	//int i = byteCount;
	
	onLED(LED0_BIT_VALUE);
	
	while(1)
	{
		offLED(LED0_BIT_VALUE);
		if(xSemaphoreTake(xEmptyCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED0_BIT_VALUE);
		
		strcpy(pMesg->data, data);
		xQueueSendToBack(xQHandle, &pMesg, (portTickType)10);
		
		if(xSemaphoreGive(xFillCount) == pdTRUE)
		{
			// Semaphore given
		}
	}
}

void vConsumer( void *pvParameters )
{
	struct msg *pMesg;
	
	onLED(LED1_BIT_VALUE);
	
	while(1)
	{
		offLED(LED1_BIT_VALUE);
		if(xSemaphoreTake(xFillCount, (portTickType)portMAX_DELAY) == pdTRUE)
		{
			// Semaphore taken
		}
		onLED(LED1_BIT_VALUE);
		
		xQueueReceive(xQHandle, &pMesg, (portTickType)10);
		usart_write_line(serialPORT_USART, pMesg->data);
		
		if(xSemaphoreGive(xEmptyCount) == pdTRUE)
		{
			// Semaphore given
		}
	}
}

int main(void)
{
	initLED();
	init_usart();
	
	xFillCount = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
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

