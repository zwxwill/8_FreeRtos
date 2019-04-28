/**
  ******************************************************************************
  * @file    bsp_do.c
  * @author  zwx
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   DI
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_do.h"


/*
*   DO1————DO6的IO口分配：
*
* DO1       DO2     DO3     DO4     DO5     DO6
* PC8       PG8     PC9     PA8     PA11    PA12
*
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the DI
  * @param  None
  * @retval None
  */
void Bsp_InitDo(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    /*
         配置所有DO关联的IO口为输出模式
    */

    /* PA8，PA11, PA12 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12; /* PA组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          /* 设为输出口 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       /* 推挽输出    */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             /* 上拉电阻使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;  /* IO口低速度 */
    GPIO_Init(GPIOA,&GPIO_InitStructure);                    /* 初始化GPIOA */
    GPIO_SetBits(GPIOA,GPIO_Pin_8 | GPIO_Pin_11| GPIO_Pin_12);  /* 上电默认使DO关断输出 */

    /* PC8, PC9 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9; /* PC组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          /* 设为输出口 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       /* 推挽输出    */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             /* 上拉电阻使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;  /* IO口低速度 */
    GPIO_Init(GPIOC,&GPIO_InitStructure);                    /* 初始化GPIOC */
    GPIO_SetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9);             /* 上电默认使DO关断输出 */

    /* PG8 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;                      /* PG组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          /* 设为输出口 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       /* 推挽输出    */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             /* 上拉电阻使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;  /* IO口低速度 */
    GPIO_Init(GPIOG,&GPIO_InitStructure);                    /* 初始化GPIOG */
    GPIO_SetBits(GPIOG,GPIO_Pin_8);
}

/**
  * @brief  单独设置某一DO通道的输出值
  * @param  DOx：被设置的DO通道，1-6
            state：DO输出状态，1代表带电输出，0代表关断输出
  * @retval None
  */
void Bsp_SetDo(uint8_t DOx,uint8_t state)
{
    switch(DOx)
    {
        case 1:
            DO1 = ~state;
            break;
        case 2:
            DO2 = ~state;
            break;
        case 3:
            DO3 = ~state;
            break;
        case 4:
            DO4 = ~state;
            break;
        case 5:
            DO5 = ~state;
            break;
        case 6:
            DO6 = ~state;
            break;
        case 7:
            //        Send_Ext_Data(DOx-7,state);
            break;
        case 8:
            //       Send_Ext_Data(DOx-7,state);
            break;
        default:
            break;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
