//NAME: 	"pwr.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS: STEROWNIK ZASILANIA

#include "pwr.h"

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
	
	//Nvic settings
	HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	
	serviceUartWriteS("\n\r#PWR INIT OK");
	
}


