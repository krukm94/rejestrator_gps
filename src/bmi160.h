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
#define BMI160_INT_MOTION0    0x5F
#define BMI160_INT_MOTION1		0x60
#define BMI160_INT_MOTION2		0x61
#define BMI160_INT_MOTION3    0x62
#define BMI160_INT_MAP0				0x55
#define BMI160_INT_MAP1				0x56
#define BMI160_INT_MAP2				0x57
#define BMI160_INT_OUT_CTRL		0x53
#define BMI160_INT_EN0				0x50
#define BMI160_INT_EN2				0x52
#define BMI160_INT_LATCH			0x54
#define BMI160_CONF						0x6A
#define BMI160_INT_STATUS0		0x1C
#define BMI160_INT_STATUS1		0x1D
#define BMI160_INT_STATUS2		0x1E
#define BMI160_INT_STATUS3		0x1F

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
void setSigMotionInt(void);
void setAnyMotionInt(void);
void setNoMotionInt(void);
void bmi160IntFunc(void);

#endif

