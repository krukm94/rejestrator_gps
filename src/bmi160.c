//NAME: 	"bmi160.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#include "bmi160.h"

//LSB defines for ACC
#define LSB_2g	 58.688890193 //ug
#define LSB_4g	 117.37089201 //ug
#define LSB_8g	 234.74178403 //ug
#define LSB_16g  469.48356807 //ug

//Handles 
SPI_HandleTypeDef spi1;

TIM_HandleTypeDef			tim4;

//Variables for avrage filter
float srednia = 0.9;
uint16_t dt   = 10;

//Variables for Acc data
volatile float rawData;
volatile float filteredData;
volatile uint8_t threshold_flag;
volatile uint8_t threshold_detection;
volatile uint8_t threshold_cnt;
									
char print_acc[80];

//Acc structure declaration
acc_axis acc;	

float acc_lsb;

//FatFS variables
extern FIL file;
 
//GPS variables
extern gps_data gps_nmea;

/**
  * @brief  Initialization BMI160
  */
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
	gpio.Mode				= GPIO_MODE_IT_FALLING;
	gpio.Pull 			= GPIO_PULLUP;
	HAL_GPIO_Init(BMI160_PORT , &gpio);
	
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
	spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
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
	
	//set acc range
	setAccRange(8);
	
	//Set No Motion interrupt
	setNoMotionInt();
	
	fifoConfig();
}

/**
  * @brief  Read bmi160 register
	* @param  addr_reg : adres of register
	*         pData    : Pointer to store read data
	*					Size     : Size of data to read
  */
void bmi160Read(uint8_t addr_reg , uint8_t* pData , uint8_t Size)
{
	addr_reg |= 0x80;																		
	
	LL_GPIO_ResetOutputPin(BMI160_PORT , BMI160_CS); 		

	HAL_SPI_Transmit(&spi1 , &addr_reg , 1 , 100);
		
	HAL_SPI_Receive(&spi1, (uint8_t *)pData , Size , 100 );	
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS); 		
}

/**
  * @brief  BMI160 Write Reg
	* @param  addr_teg : adres of register to write
	*  				pData    : Pionter to data
	*					Size		 : Size of data to send
  */
void bmi160Write(uint8_t addr_reg , uint8_t* pData , uint8_t Size)
{
	LL_GPIO_ResetOutputPin(BMI160_PORT , BMI160_CS);
	
	HAL_SPI_Transmit(&spi1 , &addr_reg , 1 , 100);
	HAL_SPI_Transmit(&spi1 , pData , 1 ,100);
	
	LL_GPIO_SetOutputPin(BMI160_PORT , BMI160_CS);
}

/**
  * @brief  Default config for bmi160
  */
void bmi160DefaultConfiguration(void)
{
	uint8_t acc_pmu_normal = 0x11;

	//Power up acc
	bmi160Write(BMI160_CMD, &acc_pmu_normal , 1);
	HAL_Delay(10);
}	
 
/**
  * @brief  Set range of accelerometer
	* @param  lsb: range of acc it can be ( 2 , 4 , 8 or 16 )
  */
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

/**
  * @brief  Read bmi160 accelerometer values
  * @param  acc_x, acc_y and acc_y are pointers to store acc data
  */
void bmi160ReadAcc(int16_t *acc_x , int16_t *acc_y , int16_t *acc_z)
{
	uint8_t read, read1 , read2;
	bmi160Read( BMI160_ACC_X , acc.acc_buf , 6);
	
	acc.acc_x = (acc.acc_buf[1] << 8) | acc.acc_buf[0];
	acc.acc_y = (acc.acc_buf[3] << 8) | acc.acc_buf[2];
	acc.acc_z = (acc.acc_buf[5] << 8) | acc.acc_buf[4];
	
	acc.X = (acc.acc_x * acc_lsb)/1000000;												
	acc.Y = (acc.acc_y * acc_lsb)/1000000;
	acc.Z = (acc.acc_z * acc_lsb)/1000000;
	
	acc.acc_g[0] = (float)sqrt(acc.X*acc.X + acc.Y*acc.Y + acc.Z*acc.Z);
	
	bmi160Read(BMI160_INT_STATUS0 , &read , 1);
	bmi160Read(BMI160_INT_STATUS1 , &read1 , 1);
	bmi160Read(BMI160_INT_STATUS2 , &read2 , 1);
	
	sprintf(print_acc,
	"\n\rX: %f [g], Y: %f [g], Z: %f [g] , G: %f [g] , INT0: 0x%0.2X  , INT1: 0x%0.2X , INT2: 0x%0.2X", 
	acc.X , acc.Y , acc.Z , acc.acc_g[0] , read, read1 , read2);
	
	serviceUartWriteS(print_acc);
}

/**
  * @brief  Set Significant Motion Interrupt in BMI160
  */
void setSigMotionInt(void)
{
	uint8_t int_motion3 	= 0x12;
	uint8_t int_map0 			= 0x04;
	uint8_t	int_out_ctrl 	= 0x08;
	uint8_t int_en 				= 0x07;
	
	//set siginificant motion interrupt enable
	bmi160Write(BMI160_INT_MOTION3 , &int_motion3 , 1);
	
	//map siginificant motion to INT 1 pin
	bmi160Write(BMI160_INT_MAP0 , &int_map0 , 1);
	
	//set INT1 Pin output
	bmi160Write(BMI160_INT_OUT_CTRL , &int_out_ctrl , 1);
	
	//set INT eN
	bmi160Write(BMI160_INT_EN0 , &int_en , 1);
}

/**
  * @brief  Set anymotion detection interrupt in bmi160
  */
void setAnyMotionInt(void)
{
	uint8_t int_motion1 	= 0x20;
	uint8_t int_ctrl 			= 0x08;
	uint8_t int_en0				= 0x07;
	uint8_t int_map0			= 0x04;
	
	//set ant motion int config
	bmi160Write(BMI160_INT_MOTION1 , &int_motion1 , 1);
	
	//ustawienie wyjscia
	bmi160Write(BMI160_INT_OUT_CTRL, &int_ctrl ,1);

	//zmapownaie int do int 1 pin
	bmi160Write(BMI160_INT_MAP0 , &int_map0 , 1);
	
	//uruchomienie przerwania
	bmi160Write(BMI160_INT_EN0 , &int_en0 , 1);
}

/**
  * @brief  Set no motion detection interrupt in bmi160
  */
void setNoMotionInt(void)
{
	uint8_t int_reset = 0xB1;
	uint8_t motion0	 	= 0x40;
	uint8_t motion2 	= 0x46;
	uint8_t motion3 	= 0x01;
	uint8_t ctrl    	= 0x08;
	uint8_t map0    	= 0x08;
	uint8_t en0     	= 0x07;
	
	//interrupt reset
	bmi160Write(BMI160_CMD , &int_reset , 1);
	
	//set time of no motion
	bmi160Write(BMI160_INT_MOTION0 , &motion0 , 1);
	
	//set threshould of no motion
	bmi160Write(BMI160_INT_MOTION2 , &motion2 , 1);
	
	//set no motion on
	bmi160Write(BMI160_INT_MOTION3 , &motion3 , 1);
	
	//set output for INT2
	bmi160Write(BMI160_INT_OUT_CTRL , &ctrl , 1);
	
	//set maping fot int 2
	bmi160Write(BMI160_INT_MAP0 , &map0 , 1);
	
	//set no motion interrup enable
	bmi160Write(BMI160_INT_EN0 , &en0 , 1);
}

/**
  * @brief  Interrupt function, it's call when interrupts occurs
  */
void bmi160IntFunc(void)
{
	uint8_t read = 0;
	bmi160Read(BMI160_INT_STATUS1 , &read , 1);
	
	if(read & 0x80)
	{
		setAnyMotionInt();
		
		//StandByMode();
	}
}

/**
  * @brief  Configuration of Fifo
  */
void fifoConfig(void)
{
	uint8_t fifo_config = 0x40;
	
	//Set enable Fifo for acc in header les mode.
	bmi160Write(BMI160_FIFO_CONFIG1 , &fifo_config , 1);
	
}

/**
  * @brief  Reading acc data from fifo
  */
void bmi160FifoAccRead(void)
{
	uint8_t fifo_length[2];
	
	acc.acc_g_size = 0;
	
	//Read fifo lvl 
	bmi160Read(BMI160_FIFO_LENGTH0 , fifo_length , 2);
	
	fifo_length[1] = (fifo_length[1] << 5);
	fifo_length[1] = (fifo_length[1] >> 5);
	
	acc.fifo_lvl = ((fifo_length[1] << 8 ) | (fifo_length[0]));
	
	acc.fifo_v = (acc.fifo_lvl/6);
	
	bmi160Read(BMI160_FIFO_DATA , acc.acc_fifo_read , acc.fifo_lvl );
}
	
/**
  * @brief  Result G vector
  */
void bmi160ResultG(void)
{
	uint16_t cnt = 0, cnt_buf = 0;
	
	for(cnt_buf = 0 ; cnt_buf < acc.fifo_v - 1 ; cnt_buf++)
	{
		if(cnt > acc.fifo_lvl) break;
		
		acc.acc_x = (acc.acc_fifo_read[1 + cnt] << 8) | acc.acc_fifo_read[0 + cnt];
		acc.acc_y = (acc.acc_fifo_read[3 + cnt] << 8) | acc.acc_fifo_read[2 + cnt];
		acc.acc_z = (acc.acc_fifo_read[5 + cnt] << 8) | acc.acc_fifo_read[4 + cnt];
		
		acc.X = (acc.acc_x * acc_lsb)/1000000;												
		acc.Y = (acc.acc_y * acc_lsb)/1000000;
		acc.Z = (acc.acc_z * acc_lsb)/1000000;
	
		acc.acc_g[cnt_buf] = (float)sqrt(acc.X*acc.X + acc.Y*acc.Y + acc.Z*acc.Z);
		acc.acc_g_size++;
		
		cnt+=6;		
	}
}


/**
  * @brief  Timer 4 Init function
*/
void tim_4_init(void)
{
	uint8_t bmi160_fifo_flush = 0xB0;
	
	__HAL_RCC_TIM4_CLK_ENABLE();
	
	tim4.Instance = TIM4;
	tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim4.Init.Prescaler = 2000 - 1;
	tim4.Init.Period = 	8500 - 1;		//4 Hz
	
	__HAL_TIM_SET_COMPARE(&tim4 , TIM_CHANNEL_1 , 1700 - 1);	//50ms  after update
	__HAL_TIM_SET_COMPARE(&tim4 , TIM_CHANNEL_2 , 3400 - 1);	//100ms after update
	__HAL_TIM_SET_COMPARE(&tim4 , TIM_CHANNEL_3 , 5100 - 1);  //150ms after update
	__HAL_TIM_SET_COMPARE(&tim4 , TIM_CHANNEL_4 , 6800 - 1);  //200ms	after update
	
	HAL_NVIC_SetPriority(TIM4_IRQn, TIM4_NVIC_PRIORITY , 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	
	__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_CC1);
	__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_CC2);
	__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_CC3);
	__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_CC4);
	
	
	bmi160Write(BMI160_CMD , &bmi160_fifo_flush , 1);
	
	if(HAL_TIM_Base_Init(&tim4) != HAL_OK){
		errorFunc("\n\r#error:bmi160.c(400):HAL_TIM_Base_Init");
	}
	__HAL_TIM_ENABLE(&tim4);
	
	serviceUartWriteS("\n\r#TIM4 INIT OK");
}

/**
  * @brief  Timer 4 interrupt function
*/
void TIM4_IRQHandler(void)
{
	/* UPDATE INTERRUPT */
	// 300 [us]
	if(__HAL_TIM_GET_FLAG(&tim4, TIM_SR_UIF))			
	{
		__HAL_TIM_CLEAR_FLAG(&tim4, TIM_SR_UIF); 
		timeMeasPinHigh();
		bmi160FifoAccRead();  
		timeMeasPinLow();
	}
	
	/* CC1 INTERUPT */
	// 138 [us]
	if(__HAL_TIM_GET_FLAG(&tim4 , TIM_SR_CC1IF))	
	{		
		__HAL_TIM_CLEAR_FLAG(&tim4, TIM_SR_CC1IF); 
		
		bmi160ResultG();
		
	}
	
	/* CC2 INTERRUPY */
	// 45 [us]
	if(__HAL_TIM_GET_FLAG(&tim4 , TIM_SR_CC2IF))	
	{
		uint16_t cnt;
		
		__HAL_TIM_CLEAR_FLAG(&tim4 , TIM_SR_CC2IF);

		for(cnt = 0 ; cnt < acc.acc_g_size ; cnt++)
		{		
			acc.acc_g_sre[cnt] = sre(acc.acc_g[cnt]);
			rawData = acc.acc_g[cnt];
			filteredData = acc.acc_g_sre[cnt];
			
			if(filteredData > ((float)1.3)) threshold_flag++;
			else threshold_flag = 0x00;
			
			if(threshold_flag == 3)
			{
				threshold_flag = 0;
				threshold_detection = 0x01;
				threshold_cnt = cnt;
			}
			else threshold_detection = 0;
		}

	}
	
	/* CC3 INTERRUPT */	
	// 0.8 [us]
	if(__HAL_TIM_GET_FLAG(&tim4 , TIM_SR_CC3IF))	
	{
		char folder_buf[40];
		
		__HAL_TIM_CLEAR_FLAG(&tim4 , TIM_SR_CC3IF);
	
		if(threshold_detection == 0x01)
		{
			threshold_detection = 0x00;
			sprintf(folder_buf , "SD:/%s/ACC", gps_nmea.date);
			
			if(f_open(&file, folder_buf , FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
			{
				f_lseek(&file , f_size(&file));
				
				sprintf(print_acc , "G: %f" , acc.acc_g_sre[threshold_cnt]);
				f_printf(&file , "\r\n Time: %s Acc %s Lat: %s Lon: %s" , 
				gps_nmea.UTC_time , print_acc , gps_nmea.latitude , gps_nmea.longitude);
				
				f_close(&file);
			}
		}
	}
	
	/* CC4 INTERRUPT */
	// 5.8 [us]
	if(__HAL_TIM_GET_FLAG(&tim4 , TIM_SR_CC4IF))	
	{
		__HAL_TIM_CLEAR_FLAG(&tim4 , TIM_SR_CC4IF);
		
		memset(acc.acc_fifo_read , 0 , sizeof(acc.acc_fifo_read));
		memset(acc.acc_g , 0 , sizeof(acc.acc_g));
		memset(acc.acc_g_sre , 0 , sizeof(acc.acc_g_sre));
	}
	
}
/**
  * @brief  Srednia kroczaca
  * @param  wynik = new sample
	*   			sre = variable to save result
  */

float sre(float wynik)
{
	srednia = srednia * dt;
	srednia = srednia + wynik;
	srednia = srednia / (dt + 1);
	
	return srednia;
}
