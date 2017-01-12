//NAME: 	"gps.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "gps.h"

UART_HandleTypeDef  gps_uart;

volatile uint8_t gpsRecive;
volatile uint16_t cnt_uart = 0;
volatile uint16_t nmea_size = 0;
volatile char buf_uart[1000];
//GPS NMEA FLAGS

volatile uint8_t gga_flag = 0;
volatile uint8_t vtg_flag = 0;

//Gps Structure
volatile gps_data gps_nmea;

//FatFS variables
FIL file;
FRESULT fres;
DIR folder;

//FatFs flags
uint8_t mkdir_flag = 0;

extern volatile uint8_t no_motion_flag;

/**
  * @brief  Uart for GPS init
  */
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
	
	gps_uart.Instance = GPS_UART_INSTANCE;
	gps_uart.Init.BaudRate = GPS_UART_BAUDRATE;
	gps_uart.Init.StopBits = UART_STOPBITS_1;
	gps_uart.Init.Parity = UART_PARITY_NONE;
	gps_uart.Init.Mode = UART_MODE_TX_RX;
	gps_uart.Init.WordLength = UART_WORDLENGTH_8B;
	gps_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	gps_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	gps_uart.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
	
	HAL_NVIC_SetPriority(GPS_UART_IRQn_NAME, UART_GPS_NVIC_PRIORITY , 0);
  HAL_NVIC_EnableIRQ(GPS_UART_IRQn_NAME);
	
	if(HAL_UART_Init(&gps_uart))
	{
		errorFunc("\n\r#error:gps_uart.c(36):HAL_UART_Init");
	}
	
	__HAL_UART_ENABLE(&gps_uart);
		
	serviceUartWriteS("\n\r#GPS UART INIT OK");
	
	LL_GPIO_SetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);
	
	gps_nmea.date[0] = '0'; //Set default date to nmea
	gps_nmea.date[1] = '1';
	gps_nmea.date[2] = '.';
	gps_nmea.date[2] = '0';
	gps_nmea.date[2] = '1';
	gps_nmea.date[2] = '.';
	gps_nmea.date[2] = '1';
	gps_nmea.date[2] = '7';
}

/**
  * @brief  EXTI1 Interrupt
  */
void EXTI1_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPS_PPS_PIN) != RESET)
  {
		__HAL_GPIO_EXTI_CLEAR_IT(GPS_PPS_PIN);
		
	}
}

/**
  * @brief  UART4 Inetrrupt
  */
void UART4_IRQHandler(void)
{		
	if(GPS_UART_INSTANCE -> ISR & UART_FLAG_RXNE)
	{
		buf_uart[cnt_uart] = GPS_UART_INSTANCE -> RDR;
		
		if((buf_uart[cnt_uart - 2] == 'V') && (buf_uart[cnt_uart - 1] == 'T') && (buf_uart[cnt_uart] == 'G')) vtg_flag = 1;

		if((vtg_flag == 1) && (buf_uart[cnt_uart] == '\r')) 
		{
			vtg_flag = 0;
			nmea_size = cnt_uart;
			
			analyzeGPS();  //20 [ms]
			
			if(no_motion_flag)
			{
				if(gps_nmea.fix_flag == 1)
				{				
					no_motion_flag = 0;
					
					serviceUartWriteS("\r\nNo motion and fix = 1");
					setAnyMotionInt();
					StandByMode();
				}
				else gps_nmea.no_fix_cnt++;
				
				if(gps_nmea.no_fix_cnt == 10)
				{
					no_motion_flag = 0;
					gps_nmea.no_fix_cnt = 0;
					
					serviceUartWriteS("\r\nNo_FIX_CNT = 10");
					setAnyMotionInt();
					StandByMode();
				}
			}
		}
		
		serviceUartWrite(buf_uart[cnt_uart]);
		
		cnt_uart++;
		if(cnt_uart == 999) cnt_uart = 0;
	}
}


/**
* @brief This function analyze GPS recive bufer
*/
void analyzeGPS(void)
{
	uint16_t cnt_for,cnt_for2,cnt_lat;
	uint8_t cn, cn_buf = 0 , comma = 0;
	
	for(cnt_for = 0 ; cnt_for < nmea_size ; cnt_for++)
	{
		//Find GGA massage
		if((buf_uart[cnt_for - 2] == 'G') && (buf_uart[cnt_for - 1] == 'G') && (buf_uart[cnt_for] == 'A'))
		{		
			memset((void *)gps_nmea.UTC_time , 0 , 9);
			//Find UTC_time
			for(cn = 0 ; cn < 8 ; cn++)
			{
				if(cn == 2 || cn == 5) gps_nmea.UTC_time[cn] = ':';
				else 
				{
					gps_nmea.UTC_time[cn] = buf_uart[cnt_for + 2 + cn_buf];
					cn_buf++;
				}
			}
			
			//Find Latitude
			for(cn = 0 ; cn < 60 ; cn++)
			{
				if(buf_uart[cnt_for + cn] == ',') comma++;
					
				if(comma == 2)
				{
					for(cnt_lat = 0 ; cnt_lat < 9 ; cnt_lat++)
					{
						if(buf_uart[cnt_for + cn + 1 + cnt_lat] == ',') break;
						gps_nmea.latitude[cnt_lat + 1] = buf_uart[cnt_for + cn + 1 + cnt_lat];
					}
				
					if(comma == 3) gps_nmea.latitude[0] = buf_uart[cnt_for + cn + 1];	
					
					if(comma == 4)
					{
						for(cnt_lat = 0 ; cnt_lat < 10 ; cnt_lat++)
						{
							if(buf_uart[cnt_for + cn + 1 + cnt_lat] == ',') break;
							gps_nmea.longitude[cnt_lat + 1] = buf_uart[cnt_for + cn + 1 + cnt_lat];
						}
					}
					if(comma == 5)
					{
						gps_nmea.longitude[0] = buf_uart[cnt_for + cn + 1];	
						break;
					}	
				}
			}
			
			//Check FIX FLAG
			for(cn=0 ; cn <50 ; cn++)
			{
				if(buf_uart[cnt_for + cn] == 'E' || buf_uart[cnt_for + cn] == 'W')
				{
					if(buf_uart[cnt_for + cn + 2] == '1')
					{
						gps_nmea.fix_flag = 1;
						break;
					}
					else gps_nmea.fix_flag = 0;
				}
			} 
					
		}
		
		//Find RMC massage
		if((buf_uart[cnt_for - 2] == 'R') && (buf_uart[cnt_for - 1] == 'M') && (buf_uart[cnt_for] == 'C'))
		{
			cn_buf = 0;
			//Find date
			for(cnt_for2 = 0 ; cnt_for2 <80 ; cnt_for2++)
			{
				if(buf_uart[cnt_for + cnt_for2] == ',') 
				{
					comma++;
					if(comma == 9)
					{
						for(cn = 0 ; cn < 8 ; cn++)
						{
							if(cn == 2 || cn == 5) gps_nmea.date[cn] = '.';
							else 
							{
								gps_nmea.date[cn] = buf_uart[cnt_for + cnt_for2 + 1 + cn_buf];
								cn_buf++;
							}
						}
					}	
				}
				
			} 
			break;
		}
	}
	
	cnt_uart = 0;
	
	// saveGpsToSd + memset = 3.8 [ms]
	if(gps_nmea.date[1] != NULL) saveGpsToSD();
	memset((void *)buf_uart , 0 , sizeof(buf_uart));
	
	
}//end void analyzeGPS(void)


/**
  * @brief  This function saves data to SD
  */
void saveGpsToSD(void)
{
	char folder_buf[20];
	uint16_t cnt_for;
	
	__HAL_UART_DISABLE_IT(&gps_uart , UART_IT_RXNE);
	
	sprintf(folder_buf , "SD:/%s" , gps_nmea.date);
	
	if((f_stat(folder_buf , NULL) != FR_OK)) f_mkdir(folder_buf);
		
	sprintf(folder_buf , "SD:/%s/GPS", gps_nmea.date);
	
	if(f_open(&file, folder_buf , FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
	{
		f_lseek(&file , f_size(&file));
		f_printf(&file , "\r\nTIME: %s\r\n", gps_nmea.UTC_time);	
		
		for(cnt_for = 0 ; cnt_for < nmea_size ; cnt_for++) f_putc(buf_uart[cnt_for] , &file);
		
		f_close(&file);
	}
	
	__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);
}


/**
  * @brief  Write one char to uart TX
  */
void gpsUartWrite(char data)
{	
	while(!(__HAL_UART_GET_FLAG(&gps_uart, UART_FLAG_TXE)));	
	GPS_UART_INSTANCE ->TDR = data;
}

/**
  * @brief  Write string to uart TX
  */
void gpsUartWriteS(char *s)
{
	while(*s)
	{
		while(!(__HAL_UART_GET_FLAG(&gps_uart, UART_FLAG_TC)));
		gpsUartWrite(*s++);
	}
}

