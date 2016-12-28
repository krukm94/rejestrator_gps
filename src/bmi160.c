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
	
	gpio.Pin				= BMI160_INT1;
	gpio.Mode				= GPIO_MODE_IT_RISING_FALLING;
	gpio.Pull 			= GPIO_PULLUP;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	gpio.Pin				= BMI160_INT2;
	gpio.Mode				= GPIO_MODE_IT_RISING_FALLING;
	gpio.Pull 			= GPIO_PULLUP;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 4);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

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
	
	//set acc range
	setAccRange(8);
	
	//set AnyMotionInterrupt
	setAnyMotionInt();
	
	//set Significant motion interrupt
	//setSigMotionInt();
	
	//set no motion interrupt
	//setNoMotionInt();
	
	//Read CHIP ID REG
	bmi160Read(BMI160_CHIP_ID , &read , 1);
	sprintf(print , "\n\r#SPI1(BMI160) INIT OK CHIP_ID: 0x%0.2X " , read);
	
	serviceUartWriteS(print);
	
	for(bmi160_init_cnt = 0 ; bmi160_init_cnt <20 ; bmi160_init_cnt++)
	{
		bmi160DefaultConfiguration();

		bmi160Read(BMI160_PMU_STATUS , &read , 1);
		if(read & (1<<4)) break;
		
		cnt++;
	}
	
	bmi160Read(BMI160_ERR , &read , 1);
	sprintf(print , "\n\r--->ERR: 0x%0.2X , cnt:%d" , read ,cnt);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_PMU_STATUS , &read , 1);
	sprintf(print , "\n\r--->STATUS PMU: 0x%0.2X " , read);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_STATUS , &read , 1);
	sprintf(print , "\n\r--->STATUS: 0x%0.2X " , read);
	serviceUartWriteS(print);
	
	bmi160Read(BMI160_ACC_CONF , &read , 1);
	sprintf(print , "\n\r--->ACC CONF: 0x%0.2X " , read);
	serviceUartWriteS(print);

	
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
	uint8_t read, read1 , read2;
	bmi160Read( BMI160_ACC_X , acc_buf , 6);
	
	acc.acc_x = (acc_buf[1] << 8) | acc_buf[0];
	acc.acc_y = (acc_buf[3] << 8) | acc_buf[2];
	acc.acc_z = (acc_buf[5] << 8) | acc_buf[4];
	
	X = (acc.acc_x * acc_lsb)/1000000;												
	Y = (acc.acc_y * acc_lsb)/1000000;
	Z = (acc.acc_z * acc_lsb)/1000000;
	
	acc.acc_g = (float)sqrt(X*X + Y*Y + Z*Z);
	
	bmi160Read(BMI160_INT_STATUS0 , &read , 1);
	bmi160Read(BMI160_INT_STATUS1 , &read1 , 1);
	bmi160Read(BMI160_INT_STATUS2 , &read2 , 1);
	
	sprintf(print_acc,
	"\n\rX: %f [g], Y: %f [g], Z: %f [g] , G: %f [g] , INT0: 0x%0.2X  , INT1: 0x%0.2X , INT2: 0x%0.2X", 
	X , Y , Z , acc.acc_g , read, read1 , read2);
	
	serviceUartWriteS(print_acc);
}

//setSigMotionInt
void setSigMotionInt(void)
{
	uint8_t int_motion3 	= 0x16;

	uint8_t int_map0 			= 0x04;
	uint8_t	int_out_ctrl 	= 0x08;
	uint8_t int_en 				= 0x07;
	uint8_t int_latch   	= 0x1C;
	
	//set siginificant motion interrupt enable
	bmi160Write(BMI160_INT_MOTION3 , &int_motion3 , 1);
	
	//map siginificant motion to INT 1 pin
	bmi160Write(BMI160_INT_MAP0 , &int_map0 , 1);
	
	//set INT1 Pin output
	bmi160Write(BMI160_INT_OUT_CTRL , &int_out_ctrl , 1);
	
	//zatrzask na 500 ms
	bmi160Write(BMI160_INT_LATCH , &int_latch , 1);
	
	//set INT eN
	bmi160Write(BMI160_INT_EN2 , &int_en , 1);
}

void setAnyMotionInt(void)
{
	uint8_t int_motion1 = 0x60;
	uint8_t int_ctrl 		= 0x08;
	uint8_t int_en0			= 0x07;
	uint8_t int_map0		= 0x04;
	uint8_t int_latch   = 0x1C;
	
	uint8_t read;
	char print[30];
	
	//set ant motion int config
	bmi160Write(BMI160_INT_MOTION1 , &int_motion1 , 1);
	
	//ustawienie wyjscia
	bmi160Write(BMI160_INT_OUT_CTRL, &int_ctrl ,1);

	//zmapownaie int do int 1 pin
	bmi160Write(BMI160_INT_MAP0 , &int_map0 , 1);
	
	//zatrzask na 500 ms
	bmi160Write(BMI160_INT_LATCH , &int_latch , 1);
	
	//uruchomienie przerwania
	bmi160Write(BMI160_INT_EN0 , &int_en0 , 1);
	
}

//setNoMotionInterrupt
void setNoMotionInt(void)
{
	uint8_t motion0 = 0x04;
	uint8_t motion2 = 0x46;
	uint8_t motion3 = 0x01;
	uint8_t ctrl    = 0x08;
	uint8_t latch   = 0x2C;
	uint8_t map0    = 0x08;
	uint8_t en2     = 0x07;
	
	//set time of no motion
	bmi160Write(BMI160_INT_MOTION0 , &motion0 , 1);
	
	//set threshould of no motion
	bmi160Write(BMI160_INT_MOTION2 , &motion2 , 1);
	
	//set no motion on
	bmi160Write(BMI160_INT_MOTION3 , &motion3 , 1);
	
	//set output for INT2
	bmi160Write(BMI160_INT_OUT_CTRL , &ctrl , 1);
	
	//set latch for interrupt
	bmi160Write(BMI160_INT_LATCH , &latch , 1);
	
	//set maping fot int 2
	bmi160Write(BMI160_INT_MAP0 , &map0 , 1);
	
	//set no motion interrup enable
	bmi160Write(BMI160_INT_EN2 , &en2 , 1);
	
	
}
//END OF FILE


