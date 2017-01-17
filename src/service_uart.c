//NAME: 	"service_uart.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "service_uart.h"

UART_HandleTypeDef  service_uart;
UART_HandleTypeDef  service_uart2;

/**
  * @brief  Init Service Uart (USART1)
  */
void serviceUartInit(void)
{
	GPIO_InitTypeDef gpio;
	
	//RCC ON
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	//GPIO INIT
	gpio.Pin = SERVICE_UART_TX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(SERVICE_UART_TX_PORT , &gpio);
	
	gpio.Pin = SERVICE_UART_RX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(SERVICE_UART_RX_PORT , &gpio);
	
	service_uart.Instance = SERVICE_UART_INSTANCE;
	service_uart.Init.BaudRate = SERVICE_UART_BAUDRATE;
	service_uart.Init.StopBits = UART_STOPBITS_1;
	service_uart.Init.Parity = UART_PARITY_NONE;
	service_uart.Init.Mode = UART_MODE_TX_RX;
	service_uart.Init.WordLength = UART_WORDLENGTH_8B;
	service_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	service_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	service_uart.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;

//	HAL_NVIC_SetPriority(SERVICE_UART_IRQn_NAME, SERVICE_UART_NVIC_PRIORITY , 0);
 // HAL_NVIC_EnableIRQ(SERVICE_UART_IRQn_NAME);
	
	if(HAL_UART_Init(&service_uart))
	{
		errorFunc(NULL);
	}
	
	__HAL_UART_ENABLE(&service_uart);

	//__HAL_UART_ENABLE_IT(&service_uart , UART_IT_RXNE);
}

/**
  * @brief  USART1 (service Uart) Irqn
  */
void USART1_IRQHandler(void)
{		
		if(SERVICE_UART_INSTANCE -> ISR & UART_FLAG_RXNE){
		gpsUartWrite(SERVICE_UART_INSTANCE -> RDR);
		
		}
}

/**
  * @brief  Wrtie char on service Uart Tx 
	* @param  char to send
  */
void serviceUartWrite(char data)
{	
	while(!(__HAL_USART_GET_FLAG(&service_uart, UART_FLAG_TXE)));	
	SERVICE_UART_INSTANCE ->TDR = data;
}

/**
  * @brief  Write string on service Uart Tx
  * @param  Pointer to string
  */
void serviceUartWriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_USART_GET_FLAG(&service_uart, UART_FLAG_TC)));
		serviceUartWrite(*s++);
		
	}
}

/**
  * @brief Init second service uart (USART3) Only Tx aviable
  */
void serviceUart2Init(void)
{
	GPIO_InitTypeDef gpio;
	
	//RCC ON
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//GPIO INIT
	gpio.Pin = SERVICE_UART2_TX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(SERVICE_UART2_TX_PORT , &gpio);
	
	service_uart2.Instance = SERVICE_UART2_INSTANCE;
	service_uart2.Init.BaudRate = SERVICE_UART2_BAUDRATE;
	service_uart2.Init.StopBits = UART_STOPBITS_1;
	service_uart2.Init.Parity = UART_PARITY_NONE;
	service_uart2.Init.Mode = UART_MODE_TX;
	service_uart2.Init.WordLength = UART_WORDLENGTH_8B;
	service_uart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	service_uart2.Init.OverSampling = UART_OVERSAMPLING_16;
	service_uart2.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;

	
	if(HAL_UART_Init(&service_uart2))
	{
		errorFunc("\n\r#error:service_uart.c(126):HAL_UART_Init");
	}
	
	__HAL_UART_ENABLE(&service_uart2);
	
}

/**
  * @brief  Wrtie char on service Uart 2 Tx 
	* @param  char to send
  */
void serviceUart2Write(char data)
{	
	while(!(__HAL_USART_GET_FLAG(&service_uart2, UART_FLAG_TXE)));	
	SERVICE_UART2_INSTANCE ->TDR = data;
}

/**
  * @brief  Write string on service Uart 2 Tx
  * @param  Pointer to string
  */
void serviceUart2WriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_USART_GET_FLAG(&service_uart2, UART_FLAG_TC)));
		serviceUart2Write(*s++);
		
	}
}
