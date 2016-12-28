//NAME: 	"main.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "main.h"



int main(void){

	HAL_Init();
	
	init();

	while(1){	
		if(!HAL_GPIO_ReadPin(BMI160_PORT , BMI160_INT2))
		{
			ledOn(4);
		}
	}
}

