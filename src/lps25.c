//NAME: 	"lps25.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "lps25.h"

SPI_HandleTypeDef spi3;

void lps25Init(void)
{
	GPIO_InitTypeDef gpio;
	uint8_t who_am_i;
	char print[30];
	
	//RCC On
	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpio.Mode 			= GPIO_MODE_AF_PP;
	gpio.Speed 			= GPIO_SPEED_FAST;
	gpio.Alternate 	= GPIO_AF6_SPI3;
	
	gpio.Pin 				= LPS25_MISO;
	HAL_GPIO_Init(LPS25_PORT , &gpio);
	
	gpio.Pin 				= LPS25_MOSI;
	HAL_GPIO_Init(LPS25_PORT , &gpio);
	
	gpio.Pin 				= LPS25_SCK;
	HAL_GPIO_Init(LPS25_PORT , &gpio);
	
	gpio.Mode 			= GPIO_MODE_OUTPUT_PP;
	gpio.Pull 			= GPIO_PULLDOWN;
	gpio.Pin 				= LPS25_CS;
	HAL_GPIO_Init(LPS25_PORT , &gpio);
	
	spi3.Instance 							= LPS25_SPI_INSTANCE;
	spi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	spi3.Init.DataSize 					= SPI_DATASIZE_8BIT;
	spi3.Init.Direction 				= SPI_DIRECTION_2LINES;
	spi3.Init.Mode 							= SPI_MODE_MASTER;
	spi3.Init.CLKPhase 					= SPI_PHASE_1EDGE;
	spi3.Init.FirstBit 					= SPI_FIRSTBIT_MSB;
	spi3.Init.CLKPolarity 			= SPI_POLARITY_LOW;
	spi3.Init.NSS 							= SPI_NSS_SOFT;
	spi3.Init.CRCPolynomial 		= 7;
	
	if(HAL_SPI_Init(&spi3))
	{
		errorFunc("\n\r#error:lps25.c(46):HAL_SPI_Init");
	}
	
	LL_GPIO_SetOutputPin(LPS25_PORT , LPS25_CS);
	
	
	//Read CHIP ID REG
	
	lps25Read(0x0F , &who_am_i , 1);
	sprintf(print , "\n\rWHP_AM_I: 0x%X " , who_am_i);
	
	serviceUartWriteS(print);
}


//lps25Read
void lps25Read(uint8_t addr_reg , uint8_t* pData , uint16_t Size)
{
	
	addr_reg |= 0x80;																	//SET READ BIT IN ADDRESS FRAME
	
	LL_GPIO_ResetOutputPin(LPS25_PORT , LPS25_CS); // SET CS LOW

	HAL_SPI_Transmit(&spi3 , &addr_reg , Size , 100);
		
	HAL_SPI_Receive(&spi3, pData , Size , 100 );	
	
	LL_GPIO_SetOutputPin(LPS25_PORT , LPS25_CS); 	// SET CS HIGH
	
}

