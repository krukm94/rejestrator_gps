//NAME: 	"pwr.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS: STEROWNIK ZASILANIA

#include "pwr.h"

extern  TIM_HandleTypeDef		tim4;
extern	TIM_HandleTypeDef		tim3;
				TIM_HandleTypeDef		tim15;

extern UART_HandleTypeDef 	service_uart;
extern UART_HandleTypeDef   gps_uart;

volatile uint16_t tim15_update;


//FatFS variables
extern FIL file;
 
//GPS variables
extern volatile gps_data gps_nmea;

//System variables
extern volatile uint32_t system_cnt;


/**
  * @brief  Power menagment init
  */
void pwrInit(void)
{
	GPIO_InitTypeDef gpio;
	
	//RCC ON
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpio.Pin = PWR_NCH_PIN;
	gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
	gpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(PWR_PORT , &gpio);
	
	gpio.Pin = PWR_NPGOOD_PIN;
	HAL_GPIO_Init(PWR_PORT , &gpio);
	
	gpio.Pin = PWR_NCE_PIN;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(PWR_PORT , &gpio);
	
	//Reset BQ24072
	LL_GPIO_ResetOutputPin(PWR_PORT , PWR_NCE_PIN);
	HAL_Delay(1);
	LL_GPIO_SetOutputPin(PWR_PORT , PWR_NCE_PIN);
	HAL_Delay(1);
	LL_GPIO_ResetOutputPin(PWR_PORT , PWR_NCE_PIN);
	
	serviceUartWriteS("\n\r#PWR INIT OK");
	
	//Timer wor wake up
	//tim_15_init();
}

/**
  * @brief  Timer 5 Init function
*/
void tim_15_init(void)
{
	__HAL_RCC_TIM15_CLK_ENABLE();
	
	tim15.Instance = TIM15;
	tim15.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim15.Init.Prescaler = 6800 - 1;
	tim15.Init.Period = 	20000 - 1;
	
	//Nvic settings 
	HAL_NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, TIM15_NVIC_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
	
	if(HAL_TIM_Base_Init(&tim15) != HAL_OK){
		errorFunc("\n\r#error:pwr.c(62):HAL_TIM_Base_Init");
	}
	
	__HAL_TIM_ENABLE(&tim15);
	
	__HAL_TIM_ENABLE_IT(&tim15, TIM_IT_UPDATE);
	
	serviceUartWriteS("\n\r#TIM15 INIT OK");
}

/**
  * @brief  Timer 5 interrupt function
*/
void TIM15_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&tim15, TIM_SR_UIF))
	{
		__HAL_TIM_CLEAR_FLAG(&tim15, TIM_SR_UIF);
		
		tim15_update++;
	}
}
/**
  * @brief  Config and enters to standbay mode
  */
void StandByMode(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);				//PA2 BMI160 INT1

	//Take off interrups 
	__HAL_TIM_DISABLE_IT(&tim3, TIM_IT_UPDATE);
	__HAL_UART_DISABLE_IT(&service_uart , UART_IT_RXNE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC1);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC2);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC3);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC4);
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
	
	saveLog("Enter StandBy");
	serviceUartWriteS("\r\n#Enter to StandBay Mode   ");
	
	//Go tu Standbay
	HAL_PWR_EnterSTANDBYMode();
}

/**
  * @brief  Config and enters in StopMode2
  */
void StopMode2(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#Enter to StopMode2    ");
	
	//Go to Stop Mode 2
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

/**
  * @brief  Config and enters in StopMode0
  */
void StopMode0(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#Enter to StopMode0   ");
	
	//Go to Stop Mode 0
	HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFE);
}

/**
  * @brief  Config and enters in SleepMode
  */
void SleepMode(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	//Config wakeup pin
	//HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off interrups 
	//__HAL_TIM_DISABLE_IT(&tim3, TIM_IT_UPDATE);
	__HAL_UART_DISABLE_IT(&service_uart , UART_IT_RXNE);
	__HAL_UART_DISABLE_IT(&gps_uart , UART_IT_RXNE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_UPDATE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC1);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC2);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC3);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_CC4);
	
	ledOff(1);
	ledOff(3);
	ledOff(4);
	
	TIM3 -> ARR = 6800 - 1; // Zmiana przerwan na 0,5 Hz
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#Enter to  Sleep MODE   ");
	
	TIM3 -> CNT = 0;
	
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON , PWR_STOPENTRY_WFI);
}

/**
  * @brief  Save log to file in SD card
  * @param  Pionter to sting which will be save in log file 
  */
void saveLog(char *s)
{
	char folder_buf[40];
	
	sprintf(folder_buf , "SD:/%s/LOG", gps_nmea.date);
	
	if(f_open(&file, folder_buf , FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
	{
		f_lseek(&file , f_size(&file));
		
		f_printf(&file , "\r\nLOG: %s Sys_cnt: %d [ms] Time: %s" , s , system_cnt , gps_nmea.UTC_time);
		
		f_close(&file);
	}
}

