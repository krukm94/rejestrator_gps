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
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPS_UART_TX_PORT , &gpio);
	
	gpio.Pin = GPS_UART_RX_PIN;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPS_UART_RX_PORT , &gpio);
	
	//PPS 
	gpio.Pin = GPS_PPS_PIN;
	gpio.Mode = GPIO_MODE_IT_RISING;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPS_PPS_PORT , &gpio);
	
	//Interrupt for PPS pin
	//Nvic settings
	HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 2);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	gps_uart.Instance = GPS_UART_INSTANCE;
	gps_uart.Init.BaudRate = GPS_UART_BAUDRATE;
	gps_uart.Init.StopBits = UART_STOPBITS_1;
	gps_uart.Init.Parity = UART_PARITY_NONE;
	gps_uart.Init.Mode = UART_MODE_TX_RX;
	gps_uart.Init.WordLength = UART_WORDLENGTH_8B;
	gps_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	gps_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	gps_uart.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
	
	if(HAL_UART_Init(&gps_uart))
	{
		errorFunc("\n\r#error:gps_uart.c(36):HAL_UART_Init");
	}
	
	__HAL_UART_ENABLE(&gps_uart);
		
	serviceUartWriteS("\n\r#GPS UART INIT OK");
	
	HAL_NVIC_SetPriority(GPS_UART_IRQn_NAME, 2, 3);
  HAL_NVIC_EnableIRQ(GPS_UART_IRQn_NAME);
	
	__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);
	
	LL_GPIO_SetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	//gpsUartWriteS("$PMTK251,38400*27\r\n");
}

//EXTI1
void EXTI1_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPS_PPS_PIN) != RESET)
  {
		__HAL_GPIO_EXTI_CLEAR_IT(GPS_PPS_PIN);
		
	}
}

//UART4_IRQHandler
void UART4_IRQHandler(void)
{		
		if(GPS_UART_INSTANCE -> ISR & UART_FLAG_RXNE){
			gpsUartRx = GPS_UART_INSTANCE -> RDR;
			//gpsUartOdczyt(usart1_rx);
			//serviceUartWrite(GPS_UART_INSTANCE -> RDR);
		}
}

//gpsUartWrite
void gpsUartWrite(char data)
{	
	while(!(__HAL_UART_GET_FLAG(&gps_uart, UART_FLAG_TXE)));	
	GPS_UART_INSTANCE ->TDR = data;
}

//gpsUartWriteS
void gpsUartWriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_UART_GET_FLAG(&gps_uart, UART_FLAG_TC)));
		gpsUartWrite(*s++);
		
		
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
