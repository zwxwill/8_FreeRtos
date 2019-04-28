/**
  ******************************************************************************
  * @file    bsp_rs485.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "bsp_rs485.h"
#include "bsp.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  RS485_TX_MODE      1
#define  RS485_RX_MODE      0

#define  COM1_UART5_485_MODE    PDout(7)    //COM1:0, Rx;1,Tx   /* 用于控制RS485处于接收还是发送模式 */
#define  COM1_UART5_485OR232    PGout(9)
#define  COM1_UART5_MB485OR232  PBout(5)

#define  COM2_UART3_485_MODE    PAout(15)   //COM2:0, Rx;1,Tx
#define  COM3_UART4_485_MODE    PDout(6)    //COM3:0, Rx;1,Tx

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param  None
  * @retval None
  */
static void bsp_UartConfig(USART_TypeDef * USARTx, char baud, char parity)
{
    USART_InitTypeDef USART_InitStructure;

    switch(baud)
    {
        case PLC_BAUDRATE_300:
            USART_InitStructure.USART_BaudRate = 300;//Set baud rate to 300
            break;
        case PLC_BAUDRATE_9600:
            USART_InitStructure.USART_BaudRate = 9600;//Set baud rate to 9600
            break;
        case PLC_BAUDRATE_38400:
            USART_InitStructure.USART_BaudRate = 38400;//Set baud rate to 38400
            break;
        case PLC_BAUDRATE_115200:
            USART_InitStructure.USART_BaudRate = 115200;//Set baud rate to 115200
            break;
    }
    switch(parity)
    {
        case PLC_PARITY_NO:
            USART_InitStructure.USART_Parity = USART_Parity_No;//No parity
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;//Set word length to 8
            break;
        case PLC_PARITY_EVEN:
            USART_InitStructure.USART_Parity = USART_Parity_Even;//Even parity
            USART_InitStructure.USART_WordLength = USART_WordLength_9b;//Set word length to 9
            break;
        case PLC_PARITY_ODD:
            USART_InitStructure.USART_Parity = USART_Parity_Odd;//Odd parity
            USART_InitStructure.USART_WordLength = USART_WordLength_9b;//Set word length to 9
            break;
    }
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//1 stop bit
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//No hardware control
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //Tx & Rx are both enabled
    USART_Init(USARTx, &USART_InitStructure);//Initialize USARTx

    /*Clear TC bit in USART_SR*/
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == SET)
    {
        USART_ClearFlag(USARTx, USART_FLAG_TC);
    }

    /*Enable transmission complete interrupt*/
    USART_ITConfig(USARTx, USART_IT_TC, ENABLE);
	
	USART_Cmd(USARTx, ENABLE);
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Rs485Com1Config(char baud, char parity, char data_bit, char stop_bit)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); //Enable clock of GPIOB, GPIOC, GPIOD, GPIOG
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//Enable clock of UART5

    /*Alternate function PC12->UART5_TX, PD2->UART5_RX*/
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);//AF, GPIOC12->UART5
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);//AF, GPIOD2->UART5

    /*Configure Pin_12 of GPIOC as AF UART5_TX*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*Configure Pin_2 of GPIOD as AF UART5_RX*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    /*Configure Pin_5 of GPIOB to select TX or RX of RS485*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
    /*Configure Pin_9 of GPIOG to select TX or RX of RS485*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);		

    /*Configure Pin_7 of GPIOD to select TX or RX of RS485*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART5_IRQ_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    bsp_UartConfig(UART5, baud, parity);

    /*Set default mode of RS485*/
	COM1_UART5_485OR232 = RS485_RX_MODE;
	COM1_UART5_MB485OR232 = RS485_RX_MODE;
    COM1_UART5_485_MODE = RS485_RX_MODE;//Default Rx
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void Usart5_IrqHandler(void)
{
	u8 res = 0;
	
    USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_RXNE);
    COM1_UART5_485_MODE = RS485_RX_MODE;  /* 发生完成后变成接收模式 */
	res = UART10->DR;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void Rs485_SendData(USART_TypeDef *uart, u8 *buf, u8 len)
{
	u8 i = 0;
	int timeout = 0;
	
	while((USART_GetFlagStatus(uart, USART_FLAG_TXE) == RESET) && (++timeout < 1000));
	COM1_UART5_485_MODE = RS485_TX_MODE;
	for(i=0; i<len; i++)
	{
		USART_SendData(uart, buf[i]);		
	}
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Rs485Com2Config(char baud, char parity, char data_bit, char stop_bit)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /*Alternate function PB10->USART3_TX, PB11->USART3_RX*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);//AF, GPIOB10->USART3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);//AF, GPIOB11->USART3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*Configure Pin_15 of GPIOA to select TX or RX of RS485*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART3_IRQ_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    bsp_UartConfig(USART3, baud, parity);

    /*Set default mode of RS485*/
    COM2_UART3_485_MODE = RS485_RX_MODE;//Default Rx
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void Usart3_IrqHandler(void)
{
    USART_ClearFlag(USART3, USART_FLAG_TC);
    COM2_UART3_485_MODE = RS485_RX_MODE;  /* 发生完成后变成接收模式 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Rs485Com3Config(char baud, char parity, char data_bit, char stop_bit)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    /*Alternate function PC10->UART4_TX, PC11->UART4_RX*/
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);//AF, GPIOC10->UART4
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);//AF, GPIOC11->UART4

    /*Configure Pin_10 of GPIOC as AF UART4_TX & Pin_11 of GPIOC as AF UART4_RX*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*Configure Pin_6 of GPIOD to select TX or RX of RS485*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART4_IRQ_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*Initialize UART4*/
    bsp_UartConfig(UART4, baud, parity);

    /*Set default mode of RS485*/
    COM3_UART4_485_MODE = RS485_RX_MODE;//Default Rx
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void Usart4_IrqHandler(void)
{
    USART_ClearFlag(UART5, USART_FLAG_TC);
    COM3_UART4_485_MODE = RS485_RX_MODE;  /* 发生完成后变成接收模式 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
int Bsp_ConfigRs485(int id, T_UartCfg *ptUartCfg)
{
    if((id != PLC_STM32_UART_COM1) && \
       (id != PLC_STM32_UART_COM2) && \
       (id != PLC_STM32_UART_COM3))
    {
        return UART_ID_ERROR;
    }

    if((ptUartCfg->mode != PLC_UART_485) && \
       (ptUartCfg->mode != PLC_UART_232))
    {
        return UART_MODE_ERROR;
    }
    if((ptUartCfg->baud != PLC_BAUDRATE_300) && \
       (ptUartCfg->baud != PLC_BAUDRATE_9600) && \
       (ptUartCfg->baud != PLC_BAUDRATE_38400) && \
       (ptUartCfg->baud != PLC_BAUDRATE_115200))
    {
        return UART_BAUD_ERROR;
    }
    if((ptUartCfg->parity != PLC_PARITY_NO) && \
       (ptUartCfg->parity != PLC_PARITY_EVEN) && \
       (ptUartCfg->parity != PLC_PARITY_ODD))
    {
        return UART_PARITYBIT_ERROR;
    }
    if(ptUartCfg->databit != PLC_DATABIT_8)
    {
        return UART_DATABIT_ERROR;
    }
    if(ptUartCfg->stopbit != PLC_STOPBIT_1)
    {
        return UART_STOPBIT_ERROR;
    }

    switch(id)
    {
        case PLC_STM32_UART_COM1:
            bsp_Rs485Com1Config(ptUartCfg->baud, ptUartCfg->parity, ptUartCfg->databit, ptUartCfg->stopbit);
            break;
        case PLC_STM32_UART_COM2:
            bsp_Rs485Com2Config(ptUartCfg->baud, ptUartCfg->parity, ptUartCfg->databit, ptUartCfg->stopbit);
            break;
        case PLC_STM32_UART_COM3:
            bsp_Rs485Com3Config(ptUartCfg->baud, ptUartCfg->parity, ptUartCfg->databit, ptUartCfg->stopbit);
            break;
    }

    return (UART_OK);
}



/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
