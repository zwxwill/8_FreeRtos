/**
  ******************************************************************************
  * @file    bsp_led.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

/* Exported functions ---------------------------------------------------------*/
void bsp_InitLed(void);
void bsp_LedOn(uint8_t _no);
void bsp_LedOff(uint8_t _no);
void bsp_LedToggle(uint8_t _no);
uint8_t bsp_IsLedOn(uint8_t _no);

#endif

/************************************** end *************************************/
