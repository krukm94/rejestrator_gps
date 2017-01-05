//NAME: 	"sd_card.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#include "sd_card.h"

//----------------------------------------------
//HANDLES
SD_HandleTypeDef sd1;
HAL_SD_CardInfoTypedef SDCardInfo1;


/**
  * @brief  Initializes the SD card device.
  * @param  None
  * @retval SD status
  */
	
uint8_t sdCardInit(void)
{
	uint8_t return_sd = SD_OK;
	uint8_t i;
	char print_sd_buf[20];
	
	GPIO_InitTypeDef sd_gpio;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_SDMMC1_CLK_ENABLE();

	sd_gpio.Pin 	= SD_D0 | SD_D1 | SD_D2 | SD_D3 | SD_CLK;
	sd_gpio.Mode			= GPIO_MODE_AF_PP;
	sd_gpio.Alternate	= GPIO_AF12_SDMMC1;
	sd_gpio.Pull	= GPIO_NOPULL;
	sd_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(SD_PORT , &sd_gpio);
	
	sd_gpio.Pin 	= GPIO_PIN_2;
	HAL_GPIO_Init(SD_CMD_PORT , &sd_gpio);
	
	sd_gpio.Pin 	= GPIO_PIN_13;
	sd_gpio.Mode  = GPIO_MODE_INPUT;
	sd_gpio.Pull	= GPIO_PULLUP;
	HAL_GPIO_Init(SD_PORT , &sd_gpio);
	
	sd1.Instance = SDMMC1;
	sd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	sd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
	sd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	sd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
	sd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
	sd1.Init.ClockDiv = 0;
	
	if (!HAL_GPIO_ReadPin(SD_PORT , SD_DETECT))
	{
		for(i=0 ; i < 10 ; i++)
		{
			return_sd = HAL_SD_Init(&sd1 , &SDCardInfo1);
			if(return_sd == SD_OK) break;
		}	
		
		if (return_sd != SD_OK)
		{
			sprintf(print_sd_buf , "\n\r#error:sd_card.c(47):HAL_SD_Init: %d " , return_sd);
			errorFunc(print_sd_buf);
		}
	}
	else
	{
			return_sd = NO_SD_CARD;
			serviceUartWriteS("\n\r#SD CARD INIT FAILED, NO SD CARD");
	}

	HAL_SD_WideBusOperation_Config(&sd1, SDMMC_BUS_WIDE_4B);
	
	serviceUartWriteS("\n\r#SD CARD INIT OK");
	
	return return_sd;
}

