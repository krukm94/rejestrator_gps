//NAME: 	"init.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "init.h"


volatile uint32_t system_cnt;

/* Fatfs structure */
FATFS FS;
FIL file;

//GPS struct
extern volatile gps_data gps_nmea;

//Print variables
char print_buf[70];

/**
  * @brief  errorFunc
  * @param  Pointer to error massage
 */
void errorFunc(char *s)
{
	char *buf_out;
	sprintf(buf_out , "\r\n\r\n###%s system_cnt: %d [ms]" , s , system_cnt);
	saveLog(buf_out);
	serviceUartWriteS(buf_out);
	
	while(1)
	{
		ledToggle(1);
		HAL_Delay(50);
	}	
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
		
		f_printf(&file , "\r\n\r\n<<<LOG: %s Sys_cnt: %d [ms] Time: %s>>>\r\n" , s , system_cnt , gps_nmea.UTC_time);
		
		f_close(&file);
	}
}


//init
void init(void)
{
	SystemClock_Config();	
	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
	serviceUartInit();
	serviceUart2Init();
	
	serviceUartWriteS("\n\r$$$$$$$$$$$$$\n\r\n\r-->REJESTRATOR GPS<--\n\r$ M.KRUK\n\r$ THIS IS SERVICE UART\n\r---------------------");
	
	gpioInit();
	
	pwrInit();
	
	f_mount(&FS, "SD:", 1);
	
	gpsUartInit();
	
	bmi160Init();
	
	//init_timers();
	
	///////////////
	//Nvic for pins interrupts
	///////////////
	//Pin nCH (BQ24072)
	HAL_NVIC_SetPriority(EXTI0_IRQn, EXTI0_NVIC_PRIORITY , 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	//Pin INT1 (BMI160) | Pin nPGOOD (BQ24072)
  HAL_NVIC_SetPriority(EXTI2_IRQn, EXTI2_NVIC_PRIORITY , 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	
	//Pin user button
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, EXTI15_NVIC_PRIORITY , 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	sprintf(print_buf , "\r\n-->System Start! Compilation Time: %s" , __TIME__);
	saveLog(print_buf);
	
	f_mount(NULL , "SD:", 1);
	SD_DeInit();
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /**Initializes the CPU, AHB and APB busses clocks 
  */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    errorFunc("\n\r#error:init.c(59):HAL_RCC_OscConfig");
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    errorFunc("\n\r#error:init.c(73):HAL_RCC_ClockConfig");
  }

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    errorFunc("\n\r#error:init.c(89):HAL_RCCEx_PeriphCLKConfig");
  }
  
    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    errorFunc("\n\r#error:init.c(96):HAL_PWREx_ControlVoltageScaling");
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, SYSTIC_NVIC_PRIOTITY, 0);
}

