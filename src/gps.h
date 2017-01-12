//NAME: 	"gps.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#ifndef __gps_H_
#define __gps_H_

#include "init.h"

typedef struct
{
	char    UTC_time[9];
	char    date[9];
	char 		latitude[10];
	char    longitude[11];
	uint8_t UTC_time_h;
	uint8_t UTC_time_m;
	uint8_t UTC_time_s;
	uint8_t fix_flag;
	uint8_t no_fix_cnt;
	
}gps_data;



void gpsUartInit(void);
void gpsUartWrite(char data);
void gpsUartWriteS(char *s);

void analyzeGPS(void);
void saveGpsToSD(void);

#endif

