//NAME: 	"gps.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#ifndef __gps_H_
#define __gps_H_

#include "init.h"

void gpsUartInit(void);
void gpsUartWrite(char data);
void gpsUartWriteS(char *s);
//void gpsUartOdczyt(uint8_t rx);

#endif

