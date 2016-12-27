//NAME: 	"bmi160.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#include "bmi160.h"

//Handles 
SPI_HandleTypeDef spi1;

//variables
uint8_t acc_buf[6];
									
char print_acc[30];
float X,Y,Z;

//deklaracja struktury acc_axis 
acc_axis acc;	

#define LSB_2g	 58.688890193 //ug
#define LSB_4g	 117.37089201 //ug
#define LSB_8g	 234.74178403 //ug
#define LSB_16g  469.48356807 //ug

float acc_lsb;

void bmi160Init(void)
{
	//GPIO handle
	GPIO_InitTypeDef gpio;
	
	uint8_t bmi160_init_cnt = 0,cnt = 0;
	uint8_t read = 0;
	
	char print[30];
	
	//RCC On
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	

	gpio.Alternate 	= GPIO_AF5_SPI1;
	gpio.Pin 				= BMI160_MISO;
	gpio.Mode 			= GPIO_MODE_AF_PP;
	gpio.Pull				= GPIO_NOPULL;
	gpio.Speed 			= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Alternate 	= GPIO_AF5_SPI1;
	gpio.Pin 				= BMI160_MOSI;
	gpio.Mode 			= GPIO_MODE_AF_PP;
	gpio.Pull				= GPIO_NOPULL;
	gpio.Speed 			= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Alternate 	= GPIO_AF5_SPI1;
	gpio.Pin 				= BMI160_SCK;
	gpio.Mode 			= GPIO_MODE_AF_PP;
	gpio.Pull				= GPIO_NOPULL;
	gpio.Speed 			= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Mode 			= GPIO_MODE_OUTPUT_PP;
	gpio.Pull 			= GPIO_PULLDOWN;
	gpio.Pin 				= BMI160_CS;
	gpio.Speed			= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	spi1.Instance 							= BMI160_SPI_INSTANCE;
	spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	spi1.Init.DataSize 					= SPI_DATASIZE_8BIT;
	spi1.Init.Direction 				= SPI_DIRECTION_2LINES;
	spi1.Init.Mode 							= SPI_MODE_MASTER;
	spi1.Init.CLKPhase 					= SPI_PHASE_2EDGE;
	spi1.Init.TIMode 						= SPI_TIMODE_DISABLE;
	spi1.Init.FirstBit 					= SPI_FIRSTBIT_MSB;
	spi1.Init.CLKPolarity 			= SPI_POLARITY_HIGH;
	spi1.Init.NSS 							= SPI_NSS_SOFT;
	spi1.Init.CRCPolynomial 		= 7;
	spi1.Init.CRCCalculation 		= SPI_CRCCALCULATION_DISABLE;
	spi1.Init.CRCLength 				= SPI_CRC_LENGTH_DATASIZE;
	spi1.Init.NSSPMode 					= SPI_NSS_PULSE_ENABLE;
	
	if(HAL_SPI_Init(&spi1))
	{
		errorFunc("\n\r#error:bmi160.c(47):HAL_SPI_Init");
	}
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS);
	
	HAL_Delay(5);
	
	//Read CHIP ID REG
	bmi160Read(BMI160_CHIP_ID , &read , 1);
	sprintf(print , "\n\r#SPI1(BMI160) INIT OK CHIP_ID: 0x%X " , read);
	
	serviceUartWriteS(print);
	
	for(bmi160_init_cnt = 0 ; bmi160_init_cnt <20 ; bmi160_init_cnt++)
	{
		bmi160DefaultConfiguration();

		bmi160Read(BMI160_PMU_STATUS , &read , 1);
		if(read & (1<<4)) break;
		
		cnt++;
	}
	
	bmi160Read(BMI160_ERR , &read , 1);
	sprintf(print , "\n\r--->ERR: 0x%X , cnt:%d" , read ,cnt);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_PMU_STATUS , &read , 1);
	sprintf(print , "\n\r--->STATUS PMU: 0x%X " , read);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_STATUS , &read , 1);
	sprintf(print , "\n\r--->STATUS: 0x%X " , read);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_ACC_CONF , &read , 1);
	sprintf(print , "\n\r--->ACC CONF: 0x%X " , read);
	serviceUartWriteS(print);

	setAccRange(8);
}

//bmiRead
void bmi160Read(uint8_t addr_reg , uint8_t* pData , uint8_t Size)
{
	addr_reg |= 0x80;																		
	
	LL_GPIO_ResetOutputPin(BMI160_PORT , BMI160_CS); 		

	HAL_SPI_Transmit(&spi1 , &addr_reg , 1 , 100);
		
	HAL_SPI_Receive(&spi1, (uint8_t *)pData , Size , 100 );	
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS); 		
}

//bmi160Write
void bmi160Write(uint8_t addr_reg , uint8_t* pData , uint8_t Size)
{
	LL_GPIO_ResetOutputPin(BMI160_PORT , BMI160_CS);
	
	HAL_SPI_Transmit(&spi1 , &addr_reg , 1 , 100);
	HAL_SPI_Transmit(&spi1 , pData , 1 ,100);
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS);
}

//bmi160DefaultConfiguration
void bmi160DefaultConfiguration(void)
{
	uint8_t acc_pmu_normal = 0x11;
	
	//Power up acc
	bmi160Write(BMI160_CMD, &acc_pmu_normal , 1);
	HAL_Delay(10);
}	
 
//setAccRange
void setAccRange(uint8_t lsb)
{
	uint8_t acc_range_2  = 0x03;
	uint8_t acc_range_4  = 0x05;
	uint8_t acc_range_8  = 0x08;
	uint8_t acc_range_16 = 0x0C;
	
	switch(lsb)
	{
		case 2:
			acc_lsb = LSB_2g;
			bmi160Write(BMI160_ACC_RANGE , &acc_range_2 , 1); 
		break;
		
		case 4:
			acc_lsb = LSB_4g;
			bmi160Write(BMI160_ACC_RANGE , &acc_range_4 , 1); 
		break;
		
		case 8:
			acc_lsb = LSB_8g;
			bmi160Write(BMI160_ACC_RANGE , &acc_range_8 , 1); 
		break;
		
		case 16:
			acc_lsb = LSB_16g;
			bmi160Write(BMI160_ACC_RANGE , &acc_range_16 , 1); 
		break;
	}
}

//bmi160ReadAcc
void bmi160ReadAcc(int16_t *acc_x , int16_t *acc_y , int16_t *acc_z)
{
	bmi160Read( BMI160_ACC_X , acc_buf , 6);
	
	acc.acc_x = (acc_buf[1] << 8) | acc_buf[0];
	acc.acc_y = (acc_buf[3] << 8) | acc_buf[2];
	acc.acc_z = (acc_buf[5] << 8) | acc_buf[4];
	
	X = (acc.acc_x * acc_lsb)/1000000;												
	Y = (acc.acc_y * acc_lsb)/1000000;
	Z = (acc.acc_z * acc_lsb)/1000000;
	
	acc.acc_g = (float)sqrt(X*X + Y*Y + Z*Z);
	
	sprintf(print_acc, "\n\rX: %f [g], Y: %f [g], Z: %f [g] , G: %f [g]", X , Y , Z , acc.acc_g);
	serviceUartWriteS(print_acc);
}

//END OF FILE


