//NAME	: "pwr.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS	: STEROWNIK ZASILANIA

#ifndef __pwr_H_
#define __pwr_H_

#include "init.h"

void pwrInit(void);
void tim_15_init(void);
void StandByMode(void);

void StopMode0(void);
void StopMode2(void);

void SleepMode(void);

void saveLog(char *s);


#endif //__pwr_H_

