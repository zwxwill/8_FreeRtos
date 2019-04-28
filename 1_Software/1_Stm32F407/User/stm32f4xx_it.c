/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "stm32f4x7_eth.h"
#include "bsp_dm9k.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* lwip includes */
#include "lwip/sys.h"

#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern xSemaphoreHandle s_xSemaphore;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void Tim2_IrqHandle(void);
extern void Tim6_IrqHandle(void);
extern void Usart1_IrqHandler(void);
extern void Usart3_IrqHandler(void);
extern void Usart5_IrqHandler(void);
extern void Usart4_IrqHandler(void);
extern void Srv_TimeAdjustInIrq(void);

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
	if (CoreDebug->DHCSR & 1)  //check C_DEBUGEN == 1 -> Debugger Connected
	{  
		__breakpoint(0);  // halt program execution here	
	}
	
	printf("enter hard fault!!!\r\n");
	
    while(1)
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
    while(1)
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
    while(1)
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
    while(1)
    {
    }
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
  * @brief
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
    Tim2_IrqHandle();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void TIM5_IRQHandler(void)
{
    Srv_TimeAdjustInIrq();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
    Tim6_IrqHandle();
}

/**
  * @brief
  * @param  None
  * @retval None
  */

/**
  * @brief
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    Usart1_IrqHandler();
}


/**
  * @brief
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
    Usart3_IrqHandler();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
    Usart4_IrqHandler();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void)
{
    Usart5_IrqHandler();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0); //清除中断线0挂起标志位

    while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == (uint32_t)Bit_RESET)
    {
        Dm9k_IrqHandle();
    }
}

/**
  * @brief  This function handles ethernet DMA interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* Frame received */
    if(ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET)
    {
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xSemaphore, &xHigherPriorityTaskWoken);
    }

    /* Clear the interrupt flags. */
    /* Clear the Eth DMA Rx IT pending bits */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);

    /* Switch tasks if necessary. */
    if(xHigherPriorityTaskWoken != pdFALSE)
    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
