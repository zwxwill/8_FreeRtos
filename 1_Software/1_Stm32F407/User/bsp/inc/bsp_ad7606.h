/**
  ******************************************************************************
  * @file    bsp_ad7606.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_AD7606_H__
#define __BSP_AD7606_H__

#include "stm32f4xx.h"
#include "bsp.h"

/* Exported functions ---------------------------------------------------------*/
#define AD7606_CS       PBout(6)
#define AD7606_BUSY     PBin(7)
#define AD7606_RESET    PBout(8)
#define AD7606_CONVST   PBout(9)

#endif

/************************************** end *************************************/



