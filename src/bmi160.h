//NAME: 	"bmi160.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#ifndef __bmi160_H_
#define __bmi160_H_

#include "init.h"

//Registers		
#define BMI160_CHIP_ID				0x00
#define BMI160_ERR						0x02
#define BMI160_PMU_STATUS			0x03
#define BMI160_ACC_X					0x12
#define BMI160_STATUS					0x1B
#define BMI160_CMD						0x7E
#define BMI160_ACC_CONF				0x40
#define BMI160_ACC_RANGE			0x41
#define BMI160_SELF_TEST			0x6D
#define BMI160_NV_CONF				0x70

//Settings
typedef struct{
		int16_t acc_x;
		int16_t acc_y;
		int16_t acc_z;
		float		acc_g;
} acc_axis;


//functions
void bmi160Init(void);
void bmi160Read(uint8_t addr_reg , uint8_t* pData , uint8_t Size);
void bmi160Write(uint8_t addr_reg , uint8_t* pData , uint8_t Size);
void bmi160DefaultConfiguration(void);
void bmi160ReadAcc(int16_t *acc_x , int16_t *acc_y , int16_t *acc_z);

void setAccRange(uint8_t lsb);

#endif

