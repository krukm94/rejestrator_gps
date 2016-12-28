//NAME: 	"init.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016


#ifndef __INIT_H_
#define __INIT_H_

#include "stm32l4xx.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_sd.h"
#include "stm32l4xx_hal_uart.h"

#include "stm32l4xx_ll_gpio.h"

#include "stdio.h"
#include <cstdio>
#include <cmath>

#include "gpio.h"
#include "sd_card.h"
#include "service_uart.h"
#include "gps.h"
#include "timer.h"
#include "pwr.h"
#include "bmi160.h"
#include "usb.h"

void SystemClock_Config(void);

void init(void);
void errorFunc(char *error_massage);


#define LED1_PORT 					GPIOC
#define LED3_PORT 					GPIOB
#define LED4_PORT 					GPIOB
#define MAINTAIN_PORT 			GPIOB


#define LED1_PIN						GPIO_PIN_7
#define LED3_PIN  					GPIO_PIN_1
#define LED4_PIN						GPIO_PIN_11
#define MAINTAIN_PIN				GPIO_PIN_9


#define SERVICE_UART_INSTANCE 		USART1
#define SERVICE_UART_BAUDRATE 		921600
#define SERVICE_UART_TX_PORT 			GPIOA
#define SERVICE_UART_RX_PORT 			GPIOB
#define SERVICE_UART_TX_PIN 			GPIO_PIN_9
#define SERVICE_UART_RX_PIN 			GPIO_PIN_7
#define SERVICE_UART_IRQn_NAME 		USART1_IRQn

#define GPS_UART_INSTANCE		UART4
#define GPS_UART_BAUDRATE 	9600
#define GPS_UART_TX_PORT 		GPIOA
#define GPS_UART_TX_PIN			GPIO_PIN_0
#define GPS_UART_RX_PORT		GPIOA
#define GPS_UART_RX_PIN			GPIO_PIN_1
#define GPS_PWR_PORT				GPIOB
#define GPS_PWR_PIN					GPIO_PIN_15
#define GPS_NRESET_PORT			GPIOC
#define GPS_NRESET_PIN			GPIO_PIN_0
#define GPS_UART_IRQn_NAME	UART4_IRQn
#define GPS_PPS_PORT				GPIOC
#define GPS_PPS_PIN					GPIO_PIN_1

#define PWR_PORT 						GPIOB

#define PWR_NCH_PIN 				GPIO_PIN_0
#define PWR_NPGOOD_PIN 			GPIO_PIN_2
#define PWR_NCE_PIN 				GPIO_PIN_13
#define	PWR_EN2_PIN					GPIO_PIN_14


#define BMI160_SPI_INSTANCE SPI1
#define BMI160_PORT 				GPIOA

#define BMI160_CS						GPIO_PIN_8
#define BMI160_MOSI					GPIO_PIN_7
#define BMI160_MISO					GPIO_PIN_6
#define	BMI160_SCK					GPIO_PIN_5
#define BMI160_INT1					GPIO_PIN_2
#define BMI160_INT2					GPIO_PIN_4


#define LPS25_SPI_INSTANCE 	SPI3
#define LPS25_PORT					GPIOB

#define LPS25_SCK						GPIO_PIN_3
#define LPS25_MISO					GPIO_PIN_4
#define LPS25_MOSI					GPIO_PIN_5
#define LPS25_CS						GPIO_PIN_6
#define LPS25_INT1					GPIO_PIN_8

#define SD_PORT 						GPIOC
#define SD_CMD_PORT					GPIOD

#define SD_D0								GPIO_PIN_8
#define SD_D1								GPIO_PIN_9
#define	SD_D2								GPIO_PIN_10
#define SD_D3								GPIO_PIN_11
#define SD_CLK							GPIO_PIN_12
#define SD_DETECT						GPIO_PIN_13
#define SD_CMD							GPIO_PIN_2

#define USB_PORT						GPIOA
#define USB_DP							GPIO_PIN_12
#define USB_DM							GPIO_PIN_11


#endif //__INIT_H_

