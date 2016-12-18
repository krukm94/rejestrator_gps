//NAME: 	"lps25.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#ifndef __lps25_H_
#define __lps25_H_

#include "init.h"

void lps25Init(void);
void lps25Read(uint8_t addr_reg , uint8_t* pData , uint16_t Size);

#endif
