//NAME: 	"gpio.c"
//AUTHOR:	M.KRUK
//DATE:		XII.2016

#include "gpio.h"

//Handles
extern  TIM_HandleTypeDef		tim4;
extern  TIM_HandleTypeDef		tim3;

extern UART_HandleTypeDef  gps_uart;

extern FATFS FS;

/**
  * @brief  Gpios init
  */
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
	
	//ts3Sel_usbDetectPinInit();
	//spi2cs_timeMeasPinInit();
	
	userButtonInit();
	
	serviceUartWriteS("\n\r#GPIO INIT OK");
}

/**
  * @brief  Set led ON
  */
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

/**
  * @brief  Set led off
  */
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

/**
  * @brief  Toggle led
  */
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


/**
  * @brief  User button init
  */
void userButtonInit(void)
{
	GPIO_InitTypeDef gpio;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio.Pin = USER_BUTTON_PIN;
	gpio.Mode = GPIO_MODE_IT_FALLING;
	gpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USER_BUTTON_PORT , &gpio);
}


/**
  * @brief  Init pin for ts3Srl /usb Detect Pin 
  */
void ts3Sel_usbDetectPinInit(void)
{
	GPIO_InitTypeDef gpio;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpio.Pin = GPIO_PIN_12;
	gpio.Mode = GPIO_MODE_IT_RISING;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB , &gpio);

}

/**
  * @brief  Init spi2 CS or time MeasPin
  */
void spi2cs_timeMeasPinInit(void)
{
	GPIO_InitTypeDef gpio;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	gpio.Pin = GPIO_PIN_4;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOC , &gpio);
	
	timeMeasPinLow();
}

/**
  * @brief  Set time measurment pin high
  */
void timeMeasPinHigh(void)
{
	LL_GPIO_SetOutputPin(GPIOC , GPIO_PIN_4);
}

/**
  * @brief  Set time measurment pin low
  */
void timeMeasPinLow(void)
{
	LL_GPIO_ResetOutputPin(GPIOC , GPIO_PIN_4);
}

/**
  * @brief  time Measurment toggle pin
  */
void timeMeasPinToggle(void)
{
	LL_GPIO_TogglePin(GPIOC , GPIO_PIN_4);
}

/**
  * @brief  UserButtonFunc
  */
void userButtonFunc(void)
{
	//Take off interrups 
	__HAL_TIM_DISABLE(&tim3);
	__HAL_TIM_DISABLE_IT(&tim3, TIM_IT_UPDATE);
	__HAL_UART_DISABLE_IT(&gps_uart , UART_IT_RXNE);
	__HAL_TIM_DISABLE_IT(&tim4, TIM_IT_UPDATE);
	
	ledOn(3);
	
	f_mount(&FS, "SD:", 1);
	MX_USB_DEVICE_Init();
}
