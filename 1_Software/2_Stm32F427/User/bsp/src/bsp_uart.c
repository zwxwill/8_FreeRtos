/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */


#include "bsp_uart.h"
#include <stdio.h>

/* Export variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define UART1_BAUD          115200
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  InitHardUart
  * @param  None
  * @retval None
  */
void bsp_InitUart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    /* 配置 USART Tx 为复用功能 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  /* 输出类型为推挽 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    /* 内部上拉电阻使能 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    /* 复用模式 */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置 USART Rx 为复用功能 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 第2步： 配置串口硬件参数 */
    USART_InitStructure.USART_BaudRate = UART1_BAUD;    /* 波特率 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);      /* 使能串口 */

    USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
}

/**
  * @brief  UartSend
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /* 等待发送结束 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    /* 写一个字节到USART1 */
    USART_SendData(USART1, (uint8_t) ch);

    return ch;
}










/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
