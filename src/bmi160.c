//NAME: 	"bmi160.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#include "bmi160.h"

SPI_HandleTypeDef spi1;

void bmi160Init(void)
{
	GPIO_InitTypeDef gpio;
	uint8_t chip_id;
	char print[30];
	
	//RCC On
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio.Mode 			= GPIO_MODE_AF_PP;
	gpio.Speed 			= GPIO_SPEED_FAST;
	gpio.Alternate 	= GPIO_AF5_SPI1;
	
	gpio.Pin 				= BMI160_MISO;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Pin 				= BMI160_MOSI;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Pin 				= BMI160_SCK;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Mode 			= GPIO_MODE_OUTPUT_PP;
	gpio.Pull 			= GPIO_PULLDOWN;
	gpio.Pin 				= BMI160_CS;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	spi1.Instance 							= BMI160_SPI_INSTANCE;
	spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	spi1.Init.DataSize 					= SPI_DATASIZE_8BIT;
	spi1.Init.Direction 				= SPI_DIRECTION_2LINES;
	spi1.Init.Mode 							= SPI_MODE_MASTER;
	spi1.Init.CLKPhase 					= SPI_PHASE_1EDGE;
	spi1.Init.FirstBit 					= SPI_FIRSTBIT_MSB;
	spi1.Init.CLKPolarity 			= SPI_POLARITY_LOW;
	spi1.Init.NSS 							= SPI_NSS_SOFT;
	spi1.Init.CRCPolynomial 		= 7;
	
	if(HAL_SPI_Init(&spi1))
	{
		errorFunc("\n\r#error:bmi160.c(47):HAL_SPI_Init");
	}
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS);
	
	//Read CHIP ID REG
	
	bmi160Read(0x00 , &chip_id , 1);
	sprintf(print , "\n\rCHIP_ID BMI160: 0x%X " , chip_id);
	
	serviceUartWriteS(print);
	
	serviceUartWriteS("\n\r#BMI160 INIT OK");
}


//bmiRead
void bmi160Read(uint8_t addr_reg , uint8_t* pData , uint16_t Size)
{
	
	addr_reg |= 0x80;																	//SET READ BIT IN ADDRESS FRAME
	
	LL_GPIO_ResetOutputPin(BMI160_PORT , BMI160_CS); // SET CS LOW

	HAL_SPI_Transmit(&spi1 , &addr_reg , Size , 100);
		
	HAL_SPI_Receive(&spi1, pData , Size , 100 );	
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS); 	// SET CS HIGH
	
}

