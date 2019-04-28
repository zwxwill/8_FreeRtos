/**
  ******************************************************************************
  * @file    bsp_ds3231.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */


#include "bsp_ds3231.h"
#include "bsp_i2c_gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include <time.h>

#define DS3231_Address (0xD0)

/* Export variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define i2cBLOCK_TIME_WAITING_FOR_INPUT ( ( portTickType ) 100 )
/* Private macro -------------------------------------------------------------*/
/* 时钟 命令 */
#define CMD_SECOND    (0x00)    /* 秒 */
#define CMD_MINUTE    (0x01)    /* 分 */
#define CMD_HOUR      (0x02)    /* 时 */
#define CMD_WEEK      (0x03)    /* 星期 */
#define CMD_DAY       (0x04)    /* 日 */
#define CMD_MONTH     (0x05)    /* 月 */
#define CMD_YEAR      (0x06)    /* 年 */
/* Private variables ---------------------------------------------------------*/
static xSemaphoreHandle s_xRtcI2cSemaphore = NULL;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
T_Calendar g_tCanlendar;

/**
  * @brief  bsp_InitLed
  * @param  None
  * @retval None
  */
void bsp_InitDs3231(void)
{
    bsp_InitI2C();

    s_xRtcI2cSemaphore = xSemaphoreCreateMutex();
    if(s_xRtcI2cSemaphore == NULL)
    {
        printf("bsp_InitDs3231 fail!\r\n");
    }
}

/**
  * @brief  bsp_Ds3231ReadByte
  * @param  None
  * @retval None
  */
u8 bsp_Ds3231ReadByte(u8 addr)
{
    u8 dat = 0u;

    i2c_Start();
    i2c_SendByte(DS3231_Address | I2C_WR);
    i2c_WaitAck();

    i2c_SendByte(addr);
    i2c_WaitAck();

    i2c_Start();
    i2c_SendByte(DS3231_Address | I2C_RD);
    i2c_WaitAck();

    dat = i2c_ReadByte();
    i2c_Stop();

    return dat;
}

/*
  * @brief  bsp_Ds3231WriteByte
  * @param  None
  * @retval None
  */
void bsp_Ds3231WriteByte(u8 addr, u8 data)
{
    i2c_Start();

    i2c_SendByte(DS3231_Address | I2C_WR);
    i2c_WaitAck();

    i2c_SendByte(addr);
    i2c_WaitAck();

    i2c_SendByte(data);
    i2c_WaitAck();

    i2c_Stop();
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ds3231GetTime(T_Calendar *ptCalendar)
{
    u8 tmp = 0;

    if(xSemaphoreTake(s_xRtcI2cSemaphore, i2cBLOCK_TIME_WAITING_FOR_INPUT) == pdTRUE)
    {
        tmp = bsp_Ds3231ReadByte(CMD_YEAR);
        ptCalendar->year = (tmp & 0x0f) + ((tmp >> 4) * 10);

        tmp = bsp_Ds3231ReadByte(CMD_MONTH);
        ptCalendar->month = (tmp & 0x0f) + (((tmp & 0x7F) >> 4) * 10);

        tmp = bsp_Ds3231ReadByte(CMD_DAY);
        ptCalendar->date = (tmp & 0x0f) + ((tmp >> 4) * 10);

        tmp = bsp_Ds3231ReadByte(CMD_WEEK);
        ptCalendar->week = tmp;

        tmp = bsp_Ds3231ReadByte(CMD_HOUR);
        ptCalendar->hour = (tmp & 0x0f) + (((tmp & 0x1F) >> 4) * 10);

        tmp = bsp_Ds3231ReadByte(CMD_MINUTE);
        ptCalendar->min = (tmp & 0x0f) + ((tmp >> 4) * 10);

        tmp = bsp_Ds3231ReadByte(CMD_SECOND);
        ptCalendar->sec = (tmp & 0x0f) + ((tmp >> 4) * 10);

        xSemaphoreGive(s_xRtcI2cSemaphore);
    }
    return ;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
uint16_t B_BCD(uint8_t val)
{
    uint8_t i = 0, j = 0, k = 0;

    i = val / 10;
    j = val % 10;
    k = j + (i << 4);

    return k;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ds3231SetTime(T_Calendar *ptCalendar)
{
    uint8_t temp = 0;

    if(xSemaphoreTake(s_xRtcI2cSemaphore, i2cBLOCK_TIME_WAITING_FOR_INPUT) == pdTRUE)
    {
        temp = B_BCD(ptCalendar->year);
        bsp_Ds3231WriteByte(CMD_YEAR, temp);

        temp = B_BCD(ptCalendar->month);
        bsp_Ds3231WriteByte(CMD_MONTH, temp);

        temp = B_BCD(ptCalendar->date);
        bsp_Ds3231WriteByte(CMD_DAY, temp);

        temp = B_BCD(ptCalendar->week);
        bsp_Ds3231WriteByte(CMD_WEEK, temp);

        temp = B_BCD(ptCalendar->hour);
        bsp_Ds3231WriteByte(CMD_HOUR, temp);

        temp = B_BCD(ptCalendar->min);
        bsp_Ds3231WriteByte(CMD_MINUTE, temp);

        temp = B_BCD(ptCalendar->sec);
        bsp_Ds3231WriteByte(CMD_SECOND, temp);

        xSemaphoreGive(s_xRtcI2cSemaphore);
    }
}


/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ds3231SetTimeFromIrq(T_Calendar *ptCalendar)
{
    uint8_t temp = 0;
    BaseType_t tTaskChangeFlg;

    if(xSemaphoreTakeFromISR(s_xRtcI2cSemaphore, &tTaskChangeFlg) == pdTRUE)
    {
        temp = B_BCD(ptCalendar->year);
        bsp_Ds3231WriteByte(CMD_YEAR, temp);

        temp = B_BCD(ptCalendar->month);
        bsp_Ds3231WriteByte(CMD_MONTH, temp);

        temp = B_BCD(ptCalendar->date);
        bsp_Ds3231WriteByte(CMD_DAY, temp);

        temp = B_BCD(ptCalendar->week);
        bsp_Ds3231WriteByte(CMD_WEEK, temp);

        temp = B_BCD(ptCalendar->hour);
        bsp_Ds3231WriteByte(CMD_HOUR, temp);

        temp = B_BCD(ptCalendar->min);
        bsp_Ds3231WriteByte(CMD_MINUTE, temp);

        temp = B_BCD(ptCalendar->sec);
        bsp_Ds3231WriteByte(CMD_SECOND, temp);

        xSemaphoreGiveFromISR(s_xRtcI2cSemaphore, &tTaskChangeFlg);
    }
}







/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
