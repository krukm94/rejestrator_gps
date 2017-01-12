//NAME: 	"bmi160.h"
//AUTHOR:	M.KRUK
//DATE 	:	XII.2016

#ifndef __bmi160_H_
#define __bmi160_H_

#include "init.h"

//Registers		
#define BMI160_CHIP_ID				((uint8_t) 0x00)
#define BMI160_ERR						((uint8_t) 0x02)
#define BMI160_PMU_STATUS			((uint8_t) 0x03)
#define BMI160_ACC_X					((uint8_t) 0x12)
#define BMI160_STATUS					((uint8_t) 0x1B)
#define BMI160_CMD						((uint8_t) 0x7E)
#define BMI160_ACC_CONF				((uint8_t) 0x40)
#define BMI160_ACC_RANGE			((uint8_t) 0x41)
#define BMI160_SELF_TEST			((uint8_t) 0x6D)
#define BMI160_NV_CONF				((uint8_t) 0x70)
#define BMI160_INT_MOTION0    ((uint8_t) 0x5F)
#define BMI160_INT_MOTION1		((uint8_t) 0x60)
#define BMI160_INT_MOTION2		((uint8_t) 0x61)
#define BMI160_INT_MOTION3    ((uint8_t) 0x62)
#define BMI160_INT_MAP0				((uint8_t) 0x55)
#define BMI160_INT_MAP1				((uint8_t) 0x56)
#define BMI160_INT_MAP2				((uint8_t) 0x57)
#define BMI160_INT_OUT_CTRL		((uint8_t) 0x53)
#define BMI160_INT_EN0				((uint8_t) 0x50)
#define BMI160_INT_EN2				((uint8_t) 0x52)
#define BMI160_INT_LATCH			((uint8_t) 0x54)
#define BMI160_CONF						((uint8_t) 0x6A)
#define BMI160_INT_STATUS0		((uint8_t) 0x1C)
#define BMI160_INT_STATUS1		((uint8_t) 0x1D)
#define BMI160_INT_STATUS2		((uint8_t) 0x1E)
#define BMI160_INT_STATUS3		((uint8_t) 0x1F)
#define BMI160_FIFO_CONFIG0   ((uint8_t) 0x46)
#define BMI160_FIFO_CONFIG1   ((uint8_t) 0x47)
#define BMI160_FIFO_DATA			((uint8_t) 0x24)
#define BMI160_FIFO_LENGTH0		((uint8_t) 0x22)
#define BMI160_FIFO_LENGTH1   ((uint8_t) 0x23)


//Settings
typedef struct{
		uint8_t 	acc_buf[6];
		uint8_t 	acc_fifo_read[160];
	  uint16_t 	fifo_lvl;
		int16_t 	acc_x;
		int16_t 	acc_y;
		int16_t 	acc_z;
		float     X;
		float		  Y;
	  float			Z;	
		float			acc_g[40];
		uint16_t  acc_g_size;
		float     acc_g_sre[40];
	  uint8_t   fifo_v;
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
void bmi160IntFromInt1(void);

void fifoConfig(void);
void bmi160FifoAccRead(void);
void bmi160ResultG(void);
void tim_4_init(void);

float sre(float wynik);

#endif //__bmi160_H_

