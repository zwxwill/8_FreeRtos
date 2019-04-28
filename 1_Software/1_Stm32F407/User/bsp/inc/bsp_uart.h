/**
  ******************************************************************************
  * @file    bsp_led.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f4xx.h"

/* PLC usart param */

/* Exported functions ---------------------------------------------------------*/
void bsp_InitUart(void);
void Uart_SendByte(u8 ch);
void Uart_SendBuf(u8 *str, u8 len);
void Uart_Printf(char *pStr);

#endif

/************************************** end *************************************/
