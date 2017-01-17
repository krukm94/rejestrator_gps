//NAME	: "pwr.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016
//OPIS	: STEROWNIK ZASILANIA

#ifndef __pwr_H_
#define __pwr_H_

#include "init.h"

void pwrInit(void);
void adcAkuInit(void);
void tim_3_init(void);

void StopMode2(void);

void SleepMode(void);
void enterToSleepMode(uint32_t Regulator);

void saveLog(char *s);

float sreAdc(float sample);


#endif //__pwr_H_

