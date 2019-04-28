/**
  ******************************************************************************
  * @file    app_check.c
  * @author  zwx
  * @version V1.0.0
  * @date    01/20/2019
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_check.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
//#include "bsp_tim.h"
#include "bsp.h"


/* Export variables ----------------------------------------------------------*/
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  timerINTERRUPT_FREQUENCY   20000   /* timer frequency: 50us one interrupt */
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param  None
  * @retval None
  */
void vSetupSysInfo(void)
{
//    bsp_TimInit(TIM6, timerINTERRUPT_FREQUENCY, TIM6_IRQ_PRI, 0);
}

/**
  * @brief  Tim6_IrqHandle.
  * @param  None
  * @retval None
  */
void Tim6_IrqHandle(void)
{
//    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
//    {
//        ulHighFrequencyTimerTicks++;
//        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
//    }
}

/**
  * @brief  vTaskCheck.
  * @param  pvParameters: task parameters
  * @retval None
  */
void vTaskCheck(void *pvParameters)
{
    uint8_t pcWriteBuffer[500];

    vSetupSysInfo();

    while(1)
    {
        printf("=================================================\r\n");
        printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
        vTaskList((char *)&pcWriteBuffer);
        printf("%s\r\n", pcWriteBuffer);

        printf("\r\n任务名       运行计数         使用率\r\n");
        vTaskGetRunTimeStats((char *)&pcWriteBuffer);
        printf("%s\r\n", pcWriteBuffer);

        vTaskDelay(3000);
    }
}


