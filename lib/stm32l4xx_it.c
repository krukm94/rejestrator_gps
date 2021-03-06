/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32l4xx_it.c
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    28-October-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern volatile uint32_t system_cnt;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern TIM_HandleTypeDef		tim3;
extern TIM_HandleTypeDef		tim4;

extern UART_HandleTypeDef 	service_uart;

extern SD_HandleTypeDef 		uSdHandle;
extern SD_CardInfo 					uSdCardInfo;

extern UART_HandleTypeDef  gps_uart;

extern volatile uint8_t gps_done_flag;
extern volatile uint8_t acc_done_flag;

extern volatile uint8_t user_button_flag;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  errorFunc("\n\r#error:HardFault_Handler");
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  errorFunc("\n\r#error:MemManage_Handler");
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  errorFunc("\n\r#error:BusFault_Handler");
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
	errorFunc("\n\r#error:UsageFault_Handler");
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();

  system_cnt++;
}


/**
* @brief This function handles USB OTG FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{

  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);  // the longes 5.3 [ms]
	
}


/**
  * @brief  This function handles SDMMC1 Handler.
  * @param  None
  * @retval None
  */
void SDMMC1_IRQHandler(void)
{

	HAL_SD_IRQHandler(&uSdHandle);  //0.6 [ms]

}


/**
* @brief This function handes EXTI0 interrupt
*/
void EXTI0_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(PWR_NCH_PIN) != RESET)
  {
		__HAL_GPIO_EXTI_CLEAR_IT(PWR_NCH_PIN);
		
	}
}


/**
  * @brief  EXTI2 IRQ Handler
  */
void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

/**
  * @brief  GPIO EXTI Callback function
  * @param  GPIO_Pin
  * @retval None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_9)
  {  
    HAL_PCDEx_BCD_VBUSDetect (&hpcd_USB_OTG_FS);
  }	
	
	//NPGOOD LOW = GOOD RANGE OF VUSB
	if(!HAL_GPIO_ReadPin(PWR_PORT , PWR_NPGOOD_PIN))
	{
		serviceUartWriteS("\r\n#NPGOOD_PIN (PB2) INTERRUPT");
	}
	
	//Wake up from Pin 4
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_WUF4))
	{
		
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF4);
		
		__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);
		__HAL_RCC_PLL_ENABLE();
		
		enableClocks();
		
		if( HAL_PWREx_DisableLowPowerRunMode() != HAL_OK)
		{
			serviceUartWriteS("\r\n\r\n Warning! Disable Low Power Run Mode \r\n");
		}
		
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4);	
		
		
		//Enable Interrupts
		__HAL_UART_ENABLE_IT(&gps_uart , UART_IT_RXNE);

		setNoMotionInt();
		fifoConfig();	
			
		__HAL_TIM_ENABLE_IT(&tim4, TIM_IT_UPDATE);
		
		__HAL_TIM_ENABLE(&tim3);
		__HAL_TIM_ENABLE_IT(&tim3, TIM_IT_UPDATE);	
		
		LL_GPIO_SetOutputPin(GPS_PWR_PORT , GPS_PWR_PIN);
		
		serviceUartWriteS("\r\n\r\n#PWR_FLAG_WUF4\r\n");
		
		

	}	
	
		//BMI160 INTERRUPT PIN 1
	if(!HAL_GPIO_ReadPin(BMI160_PORT , BMI160_INT1))
	{
		bmi160IntFromInt1();
	}
}


//EXTI15_10 IRQ Handler
void EXTI15_10_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(USER_BUTTON_PIN) != RESET)
  {
		__HAL_GPIO_EXTI_CLEAR_IT(USER_BUTTON_PIN);
		user_button_flag = 1;
	}

}


/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
