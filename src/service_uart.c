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
	
	//GPIO INIT
	gpio.Pin = SERVICE_UART_TX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FAST;
	gpio.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(SERVICE_UART_TX_PORT , &gpio);
	
	gpio.Pin = SERVICE_UART_RX_PIN;
	HAL_GPIO_Init(SERVICE_UART_RX_PORT , &gpio);
	
	service_uart.Instance = SERVICE_UART_INSTANCE;
	service_uart.Init.BaudRate = SERVICE_UART_BAUDRATE;
	service_uart.Init.StopBits = USART_STOPBITS_1;
	service_uart.Init.Parity = USART_PARITY_NONE;
	service_uart.Init.Mode = USART_MODE_TX_RX;
	
	if(HAL_UART_Init(&service_uart))
	{
		errorFunc("\n\r#error:service_uart.c(36):HAL_UART_Init");
	}
	
	__HAL_UART_ENABLE(&service_uart);
	
}

//serviceUartWrite
void serviceUartWrite(char data)
{	
	while(!(__HAL_USART_GET_FLAG(&service_uart, USART_FLAG_TXE)));	
	SERVICE_UART_INSTANCE ->TDR = data;
}

//serviceUartWriteS
void serviceUartWriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_USART_GET_FLAG(&service_uart, USART_FLAG_TC)));
		serviceUartWrite(*s++);
		
	}
}

