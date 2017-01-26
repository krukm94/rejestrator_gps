//NAME: 	"timer.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#ifndef __timer_H_
#define __timer_H_

#include "init.h"

#define timeLSB			((float )15.625) // [ns]  timeLSB = 1/64M = 15.6 [ns]

#define DISPLAY_MEAS_RES  ((uint8_t) 1)
#define NOT_DISPLAT_RES		((uint8_t) 1)

void init_timers(void);

void tim_2_init(void);

void start_tim_meas(void);
float get_tim_meas(uint8_t display_result);

#endif  //__timer_H_

