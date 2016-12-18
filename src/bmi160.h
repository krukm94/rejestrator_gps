//NAME: 	"bmi160.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#ifndef __bmi160_H_
#define __bmi160_H_


#include "init.h"


void bmi160Init(void);
void bmi160Read(uint8_t addr_reg , uint8_t* pData , uint16_t Size);

#endif
