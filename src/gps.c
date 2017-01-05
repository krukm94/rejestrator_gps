//NAME: 	"gps.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "gps.h"

UART_HandleTypeDef  gps_uart;

uint8_t gpsRecive;
int8_t cnt_uart = 0;
uint8_t gga_flag = 0;
uint8_t gga_pointer = 0;
uint8_t fix_flag = 0;
uint8_t buf_uart[600];

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
	
	LL_GPIO_SetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	HAL_Delay(10);
	
	gpsUartWriteS("$PMTK220100*2F\r\n\n");
	
	__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);

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
	if(GPS_UART_INSTANCE -> ISR & UART_FLAG_RXNE)
	{
		gpsRecive = GPS_UART_INSTANCE -> RDR;
		gpsUartOdczyt(gpsRecive);
		
		serviceUartWrite(gpsRecive);
		
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

void gpsUartOdczyt(uint8_t rx){
	
	buf_uart[cnt_uart] = rx;

	//Przeszukuje odebrany bufor szukajac "GTV"
	if(buf_uart[cnt_uart] == 'A'){	//A
		if(buf_uart[cnt_uart - 1] == 'G'){	//G
			if(buf_uart[cnt_uart - 2] == 'G'){	//G

				gga_flag = 1; //flaga wykrycia ostatniej lini
				gga_pointer = cnt_uart; 
			}
		}
	}

	if(gga_flag)
	{
			if(buf_uart[cnt_uart] == 10)
			{
				uint8_t cn;
				cnt_uart = -1;
				gga_flag = 0;
			
				for(cn=0 ; cn <50 ; cn++)
				{
					if(buf_uart[gga_pointer + cn] == 'E' || buf_uart[gga_pointer + cn] == 'W')
					{
						if(buf_uart[gga_pointer + cn + 2] == '1')
						{
							fix_flag = 1;
							break;
						}
						else fix_flag = 0;
					}
				}

				memset(buf_uart , 0 , sizeof(buf_uart));
			}
	}
	
		if(fix_flag)
	{
		
	}
	cnt_uart++;

}
