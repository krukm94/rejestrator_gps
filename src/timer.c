//NAME: 	"timer.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#include "timer.h"

TIM_HandleTypeDef			tim3;

uint8_t led_cnt = 0; 
int16_t x,y,z;

//tim_3_init
void tim_3_init(void){
		
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	tim3.Instance = TIM3;
	tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim3.Init.Prescaler = 2000;
	tim3.Init.Period = 	3200;
	
	if(HAL_TIM_Base_Init(&tim3)){
		errorFunc("\n\r#error:timer.c(20):HAL_TIM_Base_Init");
	}
	
	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 5);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
	__HAL_TIM_ENABLE(&tim3);
	
	__HAL_TIM_ENABLE_IT(&tim3, TIM_IT_UPDATE);
	
	serviceUartWriteS("\n\r#TIM3 INIT OK");
	
}

void TIM3_IRQHandler(void)
{		
	if(__HAL_TIM_GET_FLAG(&tim3, TIM_SR_UIF))
	{
		__HAL_TIM_CLEAR_FLAG(&tim3, TIM_SR_UIF);	
		
		
		led_cnt++;
		ledOff(3);
		if(!(led_cnt % 5)) bmi160ReadAcc(&x,&y,&z);
		if(!(led_cnt % 20)) 
		{
			ledOn(3);	
			ledOff(1);
			ledOff(4);
		}
	}

}
