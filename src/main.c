
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void prvPeriphClockConfig(void);
static void prvGPIOConfig(void);
static void prvUSART1Config(void);
static void prvUSART3Config(void);
void vIdleTask(void*);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) {
	prvPeriphClockConfig();
	prvGPIOConfig();
	prvUSART1Config();
	prvUSART3Config();
	xTaskCreate( vIdleTask, ( signed char * ) "Idle Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	vTaskStartScheduler();
	return 0;
}

static void prvPeriphClockConfig(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

static void prvGPIOConfig(void) {
	GPIO_InitTypeDef  xGPIODInit;

	xGPIODInit.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	xGPIODInit.GPIO_Mode = GPIO_Mode_OUT;
	xGPIODInit.GPIO_OType = GPIO_OType_PP;
	xGPIODInit.GPIO_Speed = GPIO_Speed_50MHz;
	xGPIODInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &xGPIODInit);
}

static void prvUSART1Config(void) {

	GPIO_InitTypeDef  xGPIOBInit;
	xGPIOBInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	xGPIOBInit.GPIO_Mode = GPIO_Mode_AF;
	xGPIOBInit.GPIO_Speed = GPIO_Speed_2MHz;
	xGPIOBInit.GPIO_OType = GPIO_OType_PP;
	xGPIOBInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &xGPIOBInit);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	USART_InitTypeDef xUART1Init;
	xUART1Init.USART_BaudRate = 9600;
	xUART1Init.USART_WordLength = USART_WordLength_8b;
	xUART1Init.USART_StopBits = USART_StopBits_1;
	xUART1Init.USART_Parity = USART_Parity_No;
	xUART1Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	xUART1Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &xUART1Init);

	USART_Cmd(USART1, ENABLE);
}

static void prvUSART3Config(void) {

	GPIO_InitTypeDef  xGPIODInit;
	xGPIODInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	xGPIODInit.GPIO_Mode = GPIO_Mode_AF;
	xGPIODInit.GPIO_Speed = GPIO_Speed_50MHz;
	xGPIODInit.GPIO_OType = GPIO_OType_PP;
	xGPIODInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &xGPIODInit);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	USART_InitTypeDef xUART3Init;
	xUART3Init.USART_BaudRate = 9600;
	xUART3Init.USART_WordLength = USART_WordLength_8b;
	xUART3Init.USART_StopBits = USART_StopBits_1;
	xUART3Init.USART_Parity = USART_Parity_No;
	xUART3Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	xUART3Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &xUART3Init);

	USART_Cmd(USART3, ENABLE);
}

void vIdleTask( void *pvParameters ) {
	const portTickType xDelay = 1000;
	char data = 'A';
	int count = 0;
	while(1) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		USART_SendData(USART1, data);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART3, data + 10);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	    vTaskDelay(xDelay);
	    count++;
	    if(count % 5 == 0) {
	    	data = 'A';
	    }
	    else {
	    	data++;
	    }
	}
}

/*-----------------------------------------------------------*/

void vApplicationIdleHook( void ) {
	volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 ) {
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void ) {
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName ) {
	( void ) pcTaskName;
	( void ) pxTask;
	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {
	}
}
#endif
