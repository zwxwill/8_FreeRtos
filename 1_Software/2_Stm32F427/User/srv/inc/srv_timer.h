/**
  ******************************************************************************
  * @file    srv_timer.c
  * @author  zwx
  * @version V1.1.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */
#ifndef __SRV_TIMER_H__
#define __SRV_TIMER_H__

#include "stm32f4xx.h"

/* Exported functions ---------------------------------------------------------*/
void Srv_SNTPAdjustTime(unsigned int time);
void Srv_TimeAdjustInIrq(void);
void Srv_PrintRtcTime(void);

#endif


