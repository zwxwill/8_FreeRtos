/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */


#include "bsp_led.h"


#define RCC_ALL_LED     (RCC_APB2_Periph_GPIOC)

#define GPIO_PORT_LED1  GPIOC
#define GPIO_PIN_LED1   GPIO_Pin_13


/**
  * @brief  bsp_InitLed
  * @param  None
  * @retval None
  */
void bsp_InitLed(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_AHB1PeriphClockCmd(RCC_ALL_LED, ENABLE);

//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

//    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
//    GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);
}

/**
  * @brief  bsp_LedOn
  * @param  None
  * @retval None
  */
void bsp_LedOn(uint8_t _no)
{
    _no--;

    if(_no == 0)
    {
//        GPIO_PORT_LED1->BSRRH = GPIO_PIN_LED1;
    }
}

/**
  * @brief  bsp_LedOff
  * @param  None
  * @retval None
  */
void bsp_LedOff(uint8_t _no)
{
    _no--;

    if(_no == 0)
    {
//        GPIO_PORT_LED1->BSRRL = GPIO_PIN_LED1;
    }
}

/**
  * @brief  bsp_LedToggle
  * @param  None
  * @retval None
  */
void bsp_LedToggle(uint8_t _no)
{
    if(_no == 1)
    {
//        GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
    }
}

/**
  * @brief  bsp_IsLedOn
  * @param  None
  * @retval None
  */
uint8_t bsp_IsLedOn(uint8_t _no)
{
    if(_no == 1)
    {
//        if((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 0)
        {
            return 1;
        }
        return 0;
    }

    return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
