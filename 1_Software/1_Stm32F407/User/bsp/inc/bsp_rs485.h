/**
  ******************************************************************************
  * @file    bsp_rs485.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_RS485_H__
#define __BSP_RS485_H__

#include "stm32f4xx.h"

/* Exported define -----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
enum PlcStm32UartId
{
    /*
     *  uart of stm32
     * */
    PLC_STM32_UART_COM1 = 0,
    PLC_STM32_UART_COM2,
    PLC_STM32_UART_COM3//changed, add
};

enum PlcStm32UartMode //changed, add
{
    /*
     *  mode select of uart1
     * */
    PLC_UART_485,
    PLC_UART_232
};

enum PlcUartBaudRate
{
    PLC_BAUDRATE_300,
    PLC_BAUDRATE_9600,
    PLC_BAUDRATE_38400,
    PLC_BAUDRATE_115200
};

enum PlcUartStopBit
{
    PLC_STOPBIT_1
};

enum PlcUartDataBit
{
    PLC_DATABIT_8
};

enum PlcUartParity
{
    PLC_PARITY_NO,
    PLC_PARITY_EVEN,
    PLC_PARITY_ODD
};

typedef enum
{
    UART_OK = 0,
    UART_ID_ERROR = 1,
    UART_BAUD_ERROR = 2,
    UART_DATABIT_ERROR = 3,
    UART_STOPBIT_ERROR = 4,
    UART_PARITYBIT_ERROR = 5,
    UART_MODE_ERROR = 6
} E_UartErrorCode;

#define PLC_COM1  UART5
#define PLC_COM2  USART3
#define PLC_COM3  UART4

/* 串口配置 */
typedef struct UartCfg
{
    uint8_t mode;   /* uart or rs485 */
    uint8_t baud;
    uint8_t databit;
    uint8_t stopbit;
    uint8_t parity;
} T_UartCfg;
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int Bsp_ConfigRs485(int id, T_UartCfg *ptUartCfg);
void Rs485_SendData(USART_TypeDef *uart, u8 *buf, u8 len);

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
