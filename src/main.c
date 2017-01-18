//NAME: 	"main.c"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#include "main.h"

//User Button Flags
volatile uint8_t user_button_flag;

//PWR FLAGS
volatile uint8_t go_to_stop2_mode;
volatile uint8_t go_to_sleep_mode;

//GPS FLAGS
extern volatile uint8_t analyze_flag;

//ACC flags
extern volatile uint8_t bmi_read_flag;



int main(void){

	HAL_Init();
	
	init();
	
	while(1)
	{	
		
	#if STOP_MODE_ENABLE == 1
		if(go_to_stop2_mode == 1)
		{
			go_to_stop2_mode = 0;
			StopMode2();		
		}
	#endif
		
	#if SLEEP_MODE_ENABLE == 1
		if(go_to_sleep_mode == 1)
		{
			go_to_sleep_mode = 0;
			SleepMode();
		}
	#endif
		
	#if ANALYZE_GPS_ENABLE == 1
	
		if(analyze_flag)
		{
			analyze_flag = 0;
			
			analyzeGPS();
		}
	#endif
		
	#if BMI160_DATA_OPERATIONS_ENABLE == 1
			
		if(bmi_read_flag)
		{
			bmi_read_flag = 0;
			
			bmi160Analyze();
		}
	#endif
		
	#if USER_BUTTON_ENABLE == 1
		if(user_button_flag)
		{
			user_button_flag = 0;
			
			HAL_Delay(500);
			if(!HAL_GPIO_ReadPin(USER_BUTTON_PORT , USER_BUTTON_PIN))
			{
				userButtonFunc();
			}
		}
	#endif
		
	}
}

