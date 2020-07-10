/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#include "stm32f4xx_it.h"
#include "./camera/bsp_ov2640.h"

extern DMA_HandleTypeDef DMA_Handle_dcmi;
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
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
	uint8_t i;	
	HAL_IncTick();
	for(i=0;i<NumOfTask;i++)
	{
		if(Task_Delay[i])
		{
			Task_Delay[i]--;
		}
	}
}
  

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

///**
//  * @brief  DMA中断服务函数
//  * @param  None
//  * @retval None
//  */
void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DMA_Handle_dcmi);
}


extern uint16_t img_width, img_height;
extern uint8_t fps;

/**
  * @brief  DCMI中断服务函数
  * @param  None
  * @retval None
  */
void DCMI_IRQHandler(void)
{
  if(  __HAL_DCMI_GET_FLAG(&DCMI_Handle,DCMI_FLAG_FRAMERI) == SET )    
	{
////    __HAL_DMA_DISABLE(& DMA_Handle_dcmi); 
//   	 DCMI->CR&=~(DCMI_CR_CAPTURE);       //关闭捕获
    fps++; //帧率计数
    HAL_DCMI_Stop(&DCMI_Handle);
////   __HAL_DMA_ENABLE(& DMA_Handle_dcmi);
////     /*DMA会把数据传输到液晶屏，开窗后数据按窗口排列 */
//////    ILI9806G_OpenWindow(0,0,img_width,img_height);	
    HAL_DCMI_Start_DMA(&DCMI_Handle, DCMI_MODE_CONTINUOUS, FSMC_Addr_ILI9806G_DATA,1);
    
    __HAL_DCMI_CLEAR_FLAG(&DCMI_Handle,DCMI_FLAG_FRAMERI); 
//      DCMI->CR|=DCMI_CR_CAPTURE;          //DCMI捕获使能
	}
//  
//   HAL_DCMI_Start_DMA(&DCMI_Handle, DCMI_MODE_CONTINUOUS, FSMC_Addr_ILI9806G_DATA,1);
////  OV2640_DMA_Config( FSMC_Addr_ILI9806G_DATA,1); 	
//  HAL_DCMI_IRQHandler(&DCMI_Handle);
//  fps++; //帧率计数
//  OV2640_DMA_Config( FSMC_Addr_ILI9806G_DATA,1); 
//  CAMERA_DEBUG("%d \r\n",*((uint32_t *) (FSMC_Addr_ILI9806G_DATA )));
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
