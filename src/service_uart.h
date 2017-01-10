//NAME: 	"service_uart.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#ifndef __service_uart_H_
#define __service_uart_H_

#include "init.h"

void serviceUartInit(void);
void serviceUartWrite(char data);
void serviceUartWriteS(char *s);

void serviceUart2Init(void);
void serviceUart2Write(char data);
void serviceUart2WriteS(char *s);

#endif
