/**
  ******************************************************************************
  * @file    bsp_do.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_DO_H__
#define __BSP_DO_H__

#include "stm32f4xx.h"
#include "bsp.h"

#define DO1 PCout(8)
#define DO2 PGout(8)
#define DO3 PCout(9)
#define DO4 PAout(8)
#define DO5 PAout(11)
#define DO6 PAout(12)

/* Exported functions ---------------------------------------------------------*/
void Bsp_InitDo(void);
void Bsp_SetDo(uint8_t DOx,uint8_t state);

#endif

/************************************** end *************************************/
