//NAME: 	"service_uart.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "service_uart.h"


UART_HandleTypeDef  service_uart;

//serviceUartInit
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

	
	if(HAL_UART_Init(&service_uart))
	{
		errorFunc("\n\r#error:service_uart.c(36):HAL_UART_Init");
	}
	
	__HAL_UART_ENABLE(&service_uart);
	
	HAL_NVIC_SetPriority(SERVICE_UART_IRQn_NAME, 0, 4);
  HAL_NVIC_EnableIRQ(SERVICE_UART_IRQn_NAME);

	//__HAL_UART_ENABLE_IT(&service_uart , UART_IT_RXNE);
	
}

//USART1_IRQHandler
void USART1_IRQHandler(void)
{		
		if(SERVICE_UART_INSTANCE -> ISR & UART_FLAG_RXNE){
		gpsUartWrite(SERVICE_UART_INSTANCE -> RDR);
		
		}
}

//serviceUartWrite
void serviceUartWrite(char data)
{	
	while(!(__HAL_USART_GET_FLAG(&service_uart, UART_FLAG_TXE)));	
	SERVICE_UART_INSTANCE ->TDR = data;
}

//serviceUartWriteS
void serviceUartWriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_USART_GET_FLAG(&service_uart, UART_FLAG_TC)));
		serviceUartWrite(*s++);
		
	}
}

