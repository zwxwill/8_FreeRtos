/**
  ******************************************************************************
  * @file    bsp.h
  * @author  zwx
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  */

#ifndef _BSP_H_
#define _BSP_H_


#include "stm32f10x.h"


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* 中断优先级 */
#define ETH_IRQ_PRI         6
#define EXTI0_IRQ_PRI       7
#define TIM2_IRQ_PRI        8   /* modbus定时器 */
#define TIM5_IRQ_PRI        9
#define TIM6_IRQ_PRI        10
#define UART3_IRQ_PRI       11
#define UART4_IRQ_PRI       12
#define UART5_IRQ_PRI       13




/* Exported functions ---------------------------------------------------------*/
void bsp_Init(void);


#endif

/************************************** end *************************************/
