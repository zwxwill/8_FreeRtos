/**
  ******************************************************************************
  * @file    bsp_tim.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_TIM_H__
#define __BSP_TIM_H__

#include "stm32f4xx.h"

/* Exported define -----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void bsp_TimInit(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);
void bsp_TimClose(TIM_TypeDef* TIMx);

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

