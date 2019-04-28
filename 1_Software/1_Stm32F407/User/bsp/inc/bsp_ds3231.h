/**
  ******************************************************************************
  * @file    bsp_ds3231.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_DS3231_H__
#define __BSP_DS3231_H__

#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t week;
} T_Calendar;

extern T_Calendar g_tCanlendar;

/* Exported functions ---------------------------------------------------------*/
void bsp_InitDs3231(void);
u8 bsp_Ds3231ReadByte(u8 addr);
void bsp_Ds3231WriteByte(u8 addr, u8 data);
void bsp_Ds3231GetTime(T_Calendar *ptCalendar);
void bsp_Ds3231SetTime(T_Calendar *ptCalendar);
void bsp_Ds3231SetTimeFromIrq(T_Calendar *ptCalendar);

#endif

/************************************** end *************************************/
