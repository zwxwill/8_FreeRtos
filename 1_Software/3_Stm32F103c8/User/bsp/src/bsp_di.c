/**
  ******************************************************************************
  * @file    srv_di.c
  * @author  zwx
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   DI
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_di.h"

/*
*   DI1————DI12的IO口分配：
*
* DI1       DI2     DI3     DI4     DI5     DI6     DI7     DI8     DI9     DI10        DI11        DI12
* PC2       PC3     PA0     PA3     PA4     PA5     PA6     PB0     PB1     PF11        PG6         PG7
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
void Bsp_InitDi(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    /*
        配置所有DI关联的IO口为输入模式
    */

    /* PC2，PC3 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;       /* PC组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;             /* 设为输入口 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;        /* IO口低速度 */
    GPIO_Init(GPIOC,&GPIO_InitStructure);                    /* 初始化GPIOC */

    /* PA0，PA3, PA4, PA5, PA6 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_3;       /* PA组引脚 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;             /* 设为输入口 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;         /* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;        /* IO口低速度 */
    GPIO_Init(GPIOA,&GPIO_InitStructure);                    /* 初始化GPIOA */

    /* PB0，PB1 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;     /* PB组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           /* 设为输入口 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;       /* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;      /* IO口低速度 */
    GPIO_Init(GPIOB,&GPIO_InitStructure);                  /* 初始化GPIOB */

    /* PF11 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;               /* PF组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           /* 设为输入口 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;       /* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;      /* IO口低速度 */
    GPIO_Init(GPIOF,&GPIO_InitStructure);                  /* 初始化GPIOF */

    /* PG6, PG7 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;     /* PG组引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;           /* 设为输入口 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;       /* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;      /* IO口低速度 */
    GPIO_Init(GPIOG,&GPIO_InitStructure);                  /* 初始化GPIOG */
}

/**
  * @brief  Get DI data 16位DI值，LSB为DI1，依次为DI2-DI12，DI有信号，值为1，无信号值为0，最高4位无效为1
  * @param  None
  * @retval None
  */
uint16_t Bsp_GetDiData(void)
{
    uint16_t    bsp_DI_Value=0;


    return  bsp_DI_Value;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
