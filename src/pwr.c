//NAME: 	"pwr.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS: STEROWNIK ZASILANIA

#include "pwr.h"


//pwrInit
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

//StanByMode
void StandByMode(void)
{
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);				//PA2 BMI160 INT1
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#IDE SPAC StandBay MODE   ");
	
	//Go tu Standbay
	HAL_PWR_EnterSTANDBYMode();
}


//StopMode2
void StopMode2(void)
{
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#IDE SPAC STOP2    ");
	
	
	//Go to Stop Mode 2
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

//StopMode0
void StopMode0(void)
{
	//Config PullUp for maintain PIN
	HAL_PWREx_EnablePullUpPullDownConfig();
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B , MAINTAIN_PIN);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A , BMI160_INT1);
	
	//Config wakeup pin
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);
	
	//Take off GPS
	LL_GPIO_ResetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
	
	serviceUartWriteS("\r\n#IDE SPAC STOP0   ");
	
	//Go to Stop Mode 0
	HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFE);
}
