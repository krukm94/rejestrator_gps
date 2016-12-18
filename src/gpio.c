//NAME: 	"gpio.c"
//AUTHOR:	M.KRUK
//DATE:		XII.2016

#include "gpio.h"

//gpioInit
void gpioInit(void)
{
	//RCC ON
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();	
	
	//LED1
	LL_GPIO_SetPinMode(LED1_PORT, LED1_PIN , LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED1_PORT , LED1_PIN , LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(LED1_PORT , LED1_PIN , LL_GPIO_PULL_DOWN);
	
	//LED3
	LL_GPIO_SetPinMode(LED3_PORT , LED3_PIN , LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED3_PORT , LED3_PIN , LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(LED3_PORT , LED3_PIN , LL_GPIO_PULL_DOWN);
	
	//LED4
	LL_GPIO_SetPinMode(LED4_PORT , LED4_PIN , LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED4_PORT , LED4_PIN , LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(LED1_PORT , LED1_PIN , LL_GPIO_PULL_DOWN);
	
	//MAINTAIN 
	LL_GPIO_SetPinMode(MAINTAIN_PORT, MAINTAIN_PIN , LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(MAINTAIN_PORT , MAINTAIN_PIN , LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(MAINTAIN_PORT , MAINTAIN_PIN , LL_GPIO_PULL_UP);
	
	//SET LEDS OFF
	LL_GPIO_SetOutputPin(LED1_PORT , LED1_PIN);
	LL_GPIO_SetOutputPin(LED3_PORT , LED3_PIN);
	LL_GPIO_SetOutputPin(LED4_PORT , LED4_PIN);
	
  LL_GPIO_SetOutputPin(MAINTAIN_PORT , MAINTAIN_PIN);
	
	serviceUartWriteS("\n\r#GPIO INIT OK");
}

//ledON
void ledOn(uint8_t led_nr)
{
	switch(led_nr)
	{
		case 1:
			LL_GPIO_ResetOutputPin(LED1_PORT , LED1_PIN);
			break;
		case 3:
			LL_GPIO_ResetOutputPin(LED3_PORT , LED3_PIN);
			break;
		case 4:
			LL_GPIO_ResetOutputPin(LED4_PORT , LED4_PIN);
			break;
	}
}

//ledOff
void ledOff(uint8_t led_nr)
{
	switch(led_nr)
	{
		case 1:
			LL_GPIO_SetOutputPin(LED1_PORT , LED1_PIN);
			break;
		case 3:
			LL_GPIO_SetOutputPin(LED3_PORT , LED3_PIN);
			break;
		case 4:
			LL_GPIO_SetOutputPin(LED4_PORT , LED4_PIN);
			break;
	}
}

//ledToggle
void ledToggle(uint8_t led_nr)
{
	switch(led_nr)
	{
		case 1:
			LL_GPIO_TogglePin(LED1_PORT , LED1_PIN);
			break;
		case 3:
			LL_GPIO_TogglePin(LED3_PORT , LED3_PIN);
			break;
		case 4:
			LL_GPIO_TogglePin(LED4_PORT , LED4_PIN);
			break;
	}
}
