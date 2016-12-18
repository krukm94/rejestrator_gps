//NAME: 	"gps.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "gps.h"

UART_HandleTypeDef  gps_uart;

uint8_t gpsUartRx;
//int cnt_usart1 = 0;
//uint8_t buf_usart1[1000];


void gpsUartInit(void)
{
	GPIO_InitTypeDef gpio;
	
	//RCC ON
	__HAL_RCC_UART4_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	//GPIO INIT
	//PWR for FIRRFLY
	gpio.Pin = GPS_PWR_PIN;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPS_PWR_PORT , &gpio);
	
	//TX & RX
	gpio.Pin = GPS_UART_TX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FAST;
	gpio.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPS_UART_TX_PORT , &gpio);
	
	gpio.Pin = GPS_UART_RX_PIN;
	HAL_GPIO_Init(GPS_UART_RX_PORT , &gpio);
	
	gps_uart.Instance = GPS_UART_INSTANCE;
	gps_uart.Init.BaudRate = GPS_UART_BAUDRATE;
	gps_uart.Init.StopBits = UART_STOPBITS_1;
	gps_uart.Init.Parity = UART_PARITY_NONE;
	gps_uart.Init.Mode = UART_MODE_TX_RX;
	
	if(HAL_UART_Init(&gps_uart))
	{
		errorFunc("\n\r#error:gps_uart.c(36):HAL_UART_Init");
	}
	
	NVIC_SetPriority(GPS_IRQn_NAME, 2);
	NVIC_EnableIRQ(GPS_IRQn_NAME);
	
	LL_GPIO_SetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	__HAL_UART_ENABLE(&gps_uart);
	
	__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);
	
	serviceUartWriteS("\n\r#GPS UART INIT OK");
}

void UART4_IRQHandler(void)
{		
		if(GPS_UART_INSTANCE -> ISR & UART_FLAG_RXNE){
			gpsUartRx = GPS_UART_INSTANCE -> RDR;
			//gpsUartOdczyt(usart1_rx);
			//serviceUartWrite(GPS_UART_INSTANCE -> RDR);
		}
}

//void gpsUartOdczyt(uint8_t rx){
//	
//		buf_usart1[cnt_usart1] = rx;

//	
//		//Przeszukuje odebrany bufor szukajac "GTV"
//		if(buf_usart1[cnt_usart1] == 71){	//G
//			if(buf_usart1[cnt_usart1 - 1] == 84){	//T
//				if(buf_usart1[cnt_usart1 - 2] == 86){	//V

//								last_line_usart1 = 1; //flaga wykrycia ostatniej lini
//				}
//			}
//		}

//		
//		// wyszukiwanie znaku New Line		
//		if(last_line_usart1){
//				if(buf_usart1[cnt_usart1] == 10){
//					
//					uint16_t i;
//					// - 1 poniewaz na koncu funkcji jest cnt++ a chcemy wejsc wchodizc w funkcjie z wartoscia 0;
//					cnt_usart1 = -1;
//					last_line_usart1 = 0;
//				
//					//uart4_WriteS(buf_usart1);
//					// niezbedne wyczyszczenie smieci z buforu
//						for(i=0;i<1000;i++){
//							buf_usart1[i] = 0;
//						}
//				}
//		}
//		cnt_usart1++;

//}
