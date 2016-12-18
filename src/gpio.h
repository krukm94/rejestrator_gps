//NAME: 	"gpio.h"
//AUTHOR:	M.KRUK
//DATE:		XII.2016

#ifndef __gpio_H__
#define __gpio_H__

#include "init.h"

void 		gpioInit(void);

void 		ledOn(uint8_t led_nr);
void 		ledOff(uint8_t led_nr);
void		ledToggle(uint8_t led_nr);

#endif  //__gpio_H__
