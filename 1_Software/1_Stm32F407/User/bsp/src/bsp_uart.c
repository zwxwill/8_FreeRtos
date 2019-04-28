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
#include "bsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "srv_printf.h"

/* Export variables ----------------------------------------------------------*/
QueueHandle_t xQueueUart = NULL;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define UART1_BAUD          115200
#define USART1_DEBUG_HEAD   (0x60)
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
	NVIC_InitTypeDef NVIC_InitStructure;

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

    USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_IRQ_PRI; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
    NVIC_Init(&NVIC_InitStructure); 
	
	/* create mbox */
	xQueueUart = xQueueCreate(10, sizeof(u8));
	if(xQueueUart == NULL)
	{
		SRV_PRINTF(SRV_PRINTF_NOW, ("xQueueUart can't create \r\n"));
	}
	
	USART_Cmd(USART1, ENABLE);      /* 使能串口 */
}

/**
  * @brief  UartSend
  * @param  None
  * @retval None
  */
void Uart_SendByte(u8 ch)
{
    /* 等待发送结束 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    /* 写一个字节到USART1 */
    USART_SendData(USART1, (uint8_t) ch);
}

/**
  * @brief  Uart_SendBuf
  * @param  None
  * @retval None
  */
void Uart_SendBuf(u8 *str, u8 len)
{
	int i = 0;
	
	for(i=0; i<len; i++)
	{
		/* 等待发送结束 */
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

		/* 写一个字节到USART1 */
		USART_SendData(USART1, (uint8_t) str[i]);		
	}
}

/**
  * @brief  Uart_Printf
  * @param  None
  * @retval None
  */
void Uart_Printf(char *pStr)
{
	u32 i = 0;
	
	if(pStr != (char *)0)
	{
		while(pStr[i] != '\0')
		{			
			Uart_SendByte(pStr[i++]);			
		}
	}
}


/**
  * @brief  This can be used by usart or by ram printf
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

/**
  * @brief  0x60 + cmd + sum ( cmd = 0xA1 / 0xA2 )
  * @param  None
  * @retval None
  */
void Usart1_IrqHandler(void)
{
	static u8 head = 0;
	static u8 cmd = 0;
	u8 res = 0;
	BaseType_t xHighPrioWoken = pdFALSE;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART1);

		if((res == USART1_DEBUG_HEAD) && (head == 0))
		{
			head = 1;
		}
		else if((head == 1) && (res == 0xA1)) // cmd = 0xA1 : printf memory
		{
			head = 2;
			cmd = res;
		}
		else if((head == 1) && (res == 0xA2)) // cmd = 0xA2 : printf task state
		{
			head = 2;
			cmd = res;
		}
		else if(head == 2) // check sum 
		{
			if(res == (u8)(USART1_DEBUG_HEAD + cmd))
			{
				// send queue
				xQueueSendFromISR(xQueueUart, (void *)(&cmd), &xHighPrioWoken);
			}
			head = 0;
			cmd = 0;
		}
		else // not what my want
		{
			head = 0;
			cmd = 0;
		} 			
	}
}








/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
