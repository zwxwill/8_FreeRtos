/**
  ******************************************************************************
  * @file    bsp_sram.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_SRAM_H__
#define __BSP_SRAM_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported define -----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported struct  ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void bsp_InitSram(void);
void bsp_SramWrite(u8* pBuffer,u32 WriteAddr,u32 n);
void bsp_SramRead(u8* pBuffer,u32 ReadAddr,u32 n);

#ifdef __cplusplus
}
#endif

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


