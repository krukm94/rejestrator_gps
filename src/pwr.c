//NAME: 	"pwr.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS: STEROWNIK ZASILANIA

#include "pwr.h"

ADC_HandleTypeDef hadc1;

extern  TIM_HandleTypeDef		tim4;
TIM_HandleTypeDef			tim3;
volatile uint32_t tim3_cnt;

extern UART_HandleTypeDef 	service_uart;
extern UART_HandleTypeDef   gps_uart;

extern volatile uint8_t go_to_sleep_mode;
 
//GPS variables
extern volatile gps_data gps_nmea;

//System variables
extern volatile uint32_t system_cnt;

//Variables for ADC 
float v_ref = 2.99;
volatile uint16_t adc_value;

float adc_resolution = 4096.0;
uint16_t average_adc = 2525; // ok 3.7 V

uint16_t v_aku_adc; //v_aku in adc value
float v_aku;		//v_aku in [V]
uint8_t dt_adc = 10;

volatile uint8_t low_aku_voltage;
volatile uint8_t low_v_aku_cnt;
volatile uint8_t aku_discharge_flag;

char print[40];

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
	
	adcAkuInit();
	
	tim_3_init();
	
	serviceUartWriteS("\n\r#PWR INIT OK");
}


/**
  * @brief  Config and enters in StopMode2
  */
void StopMode2(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	go_to_sleep_mode = 0;
	
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off interrups 
	__HAL_TIM_DISABLE(&tim3);
	__HAL_TIM_DISABLE_IT(&tim3, TIM_IT_UPDATE);
	__HAL_UART_DISABLE_IT(&service_uart , UART_IT_RXNE);
	__HAL_UART_DISABLE_IT(&gps_uart , UART_IT_RXNE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_UPDATE);
	
	// Clear Flags 
	__HAL_TIM_CLEAR_FLAG(&tim3, TIM_SR_UIF);
	__HAL_TIM_CLEAR_FLAG(&tim4, TIM_SR_UIF);
	__HAL_GPIO_EXTI_CLEAR_IT(PWR_NCH_PIN);
	__HAL_GPIO_EXTI_CLEAR_IT(BMI160_INT2);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	UART4 -> RQR |= ( 1 << 3);				//Clear RXEN flag
	USART1 -> RQR |= ( 1 << 3);				//Clear RXEN flag
	//RCC -> CSR |= ( 0 << 23 );			//Clear BOR flag

	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	//Off leds
	ledOff(1);
	ledOff(3);
	ledOff(4);
	
	//Print Logs
	serviceUartWriteS("\r\n#Enter to StopMode2    ");
	saveLog("Enter to StopMode2");
	
	//Cleat WUF4 flag
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
	
	RCC -> CFGR |= (0 << 15);					//Set MSI as wakeup clock
	
	  /* Set Stop mode 2 */
  MODIFY_REG(PWR->CR1, PWR_CR1_LPMS, PWR_CR1_LPMS_STOP2);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
	
	/* Reset SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
	
	//<<<<<<<<<<<<<<< AFTER EXIT STOP MODE 2 >>>>>>>>>>>>

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
	
	if( HAL_PWREx_DisableLowPowerRunMode() != HAL_OK)
	{
		serviceUartWriteS("\r\n\r\n Warning! Disable Low Power Run Mode \r\n");
	}
}

/**
  * @brief  Config and enters in SleepMode
  */
void SleepMode(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	
	//Take off interrups 
	__HAL_TIM_DISABLE(&tim3);
	__HAL_TIM_DISABLE_IT(&tim3, TIM_IT_UPDATE);
	
	ledOff(1);
	ledOff(3);
	//ledOff(4);
	
	serviceUartWriteS("\r\n#Enter to  Sleep MODE   ");
	
	enterToSleepMode(PWR_MAINREGULATOR_ON);
	
	// <<<<<<<<<<<<<< AFTER WAKE UP >>>>>>>>>>>>>>>>>>>>>>>>>.
	__HAL_TIM_ENABLE(&tim3);
	__HAL_TIM_ENABLE_IT(&tim3, TIM_IT_UPDATE);
}

/**
  * @brief  Enter To sleep Mode
	* @param  Regulator: Specifhy witch regulator is in use, MAIN or LPR 
  */
void enterToSleepMode(uint32_t Regulator)
{
	  /* Set Regulator parameter */
  if (Regulator == PWR_MAINREGULATOR_ON)
  {
    /* If in low-power run mode at this point, exit it */
    if (HAL_IS_BIT_SET(PWR->SR2, PWR_SR2_REGLPF))
    {
      HAL_PWREx_DisableLowPowerRunMode();  
    } 
    /* Regulator now in main mode. */
  }
  else
  {
    /* If in run mode, first move to low-power run mode.
       The system clock frequency must be below 2 MHz at this point. */
    if (HAL_IS_BIT_SET(PWR->SR2, PWR_SR2_REGLPF) == RESET)
    {
      HAL_PWREx_EnableLowPowerRunMode();  
    } 
  } 
    
  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}

/**
  * @brief  Init ADC for monitor aku voltage
  */
void adcAkuInit(void)
{
	GPIO_InitTypeDef gpio;
	ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef sConfig;
	
	__HAL_RCC_ADC_CLK_ENABLE();

	gpio.Pin = GPIO_PIN_3;
	gpio.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio);
	
    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
	
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    errorFunc("\r\n#error:pwr.c(194) HAL_ADC_Init");
  }

    /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    errorFunc("\r\n#error:pwr.c(200) HAL_ADCEx_MultiModeConfigChannel");
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
	
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    errorFunc("\r\n#error.pwr.c(213) HAL_ADC_ConfigChannel");
  }
	
	
	serviceUartWriteS("\r\n#ADC INIT OK");
}

/**
  * @brief  Timer 3 Init function
*/
void tim_3_init(void)
{
		
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	tim3.Instance = TIM3;
	tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim3.Init.Prescaler = 20000 - 1;
	tim3.Init.Period = 	340 - 1;
	
	//Nvic settings 
	HAL_NVIC_SetPriority(TIM3_IRQn, TIM3_NVIC_PRIORITY, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
	if(HAL_TIM_Base_Init(&tim3) != HAL_OK){
		errorFunc("\n\r#error:timer.c(20):HAL_TIM_Base_Init");
	}
	
	__HAL_TIM_ENABLE(&tim3);
	
	__HAL_TIM_ENABLE_IT(&tim3, TIM_IT_UPDATE);
	
	serviceUartWriteS("\n\r#TIM3 INIT OK");
}

/**
  * @brief  Timer 3 interrupt function
*/
void TIM3_IRQHandler(void)
{		
	if(__HAL_TIM_GET_FLAG(&tim3, TIM_SR_UIF))
	{
		__HAL_TIM_CLEAR_FLAG(&tim3, TIM_SR_UIF);	
		
		tim3_cnt++;
		ledOn(4);
		//Battery voltage measurment
		if(!(tim3_cnt % 20))
		{	
			adc_value = HAL_ADC_GetValue(&hadc1);
			
			v_aku_adc = sreAdc(adc_value);
			
			v_aku = 2*((v_ref * v_aku_adc )/adc_resolution);
			
			if((v_aku <= ((float )3.4)) && (v_aku > ((float) 3.2))) low_aku_voltage = 1; 
			else low_aku_voltage = 0;
			
			if(v_aku <= ((float) 3.2)) low_v_aku_cnt++;
			
			if(low_v_aku_cnt == 5) aku_discharge_flag = 1;
			else aku_discharge_flag = 0;
			
			if(low_aku_voltage) 
			{
				serviceUartWriteS("\r\n#Low aku voltage");
				sprintf(print , "Low aku voltage V_Aku: %0.2f " , v_aku);
				saveLog(print);
			}
			else if(aku_discharge_flag) 
			{
				serviceUartWriteS("\r\n#Aku discharge! ");
				sprintf(print , "Aku discharge! V_Aku: %0.2f " , v_aku);
				saveLog(print);
			}
			else if(v_aku < ((float) 0.5))
			{
				saveLog("No Aku");
			}
			else
			{
			sprintf(print , "V_Aku: %0.2f " , v_aku);
			saveLog(print);
			}
			
			HAL_ADC_Start(&hadc1);
		}
		
		//Check Flag
		if(aku_discharge_flag)
		{
//			sprintf(print , "Aku discharge! POWER DOWN! V_Aku: %0.2f " , v_aku);
//			saveLog(print);
//			
//			LL_GPIO_ResetOutputPin(MAINTAIN_PORT , MAINTAIN_PIN);
		}
	}
}

/**
  * @brief  Srednia kroczaca
  * @param  sample = new sample
	*   			average = variable to save result
  */
float sreAdc(float sample)
{
	average_adc = average_adc * dt_adc;
	average_adc = average_adc + sample;
	average_adc = average_adc / (dt_adc + 1);
	
	return average_adc;
}



