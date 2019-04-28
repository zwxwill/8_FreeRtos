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
#include "bsp_tim.h"
#include "bsp.h"

#include "srv_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lwip/stats.h"
#include "arch/sys_arch.h"
#include "heap_6.h"

/* Export variables ----------------------------------------------------------*/
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;
extern QueueHandle_t xQueueUart;
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
    bsp_TimInit(TIM6, timerINTERRUPT_FREQUENCY, TIM6_IRQ_PRI, 0);  /* 用于统计任务使用情况 */
}

/**
  * @brief  Tim6_IrqHandle.
  * @param  None
  * @retval None
  */
void Tim6_IrqHandle(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        ulHighFrequencyTimerTicks++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void DispTaskInfo(void)
{
	uint8_t pcWriteBuffer[512] = {0}; 
	
	printf("Task Info------\n");
	printf("Name\t\tState\tPrio\tLeftS\tTaskNo\n");
	vTaskList((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);
	
	printf("Name\t\tRunCnt\t\tUseRate\n");
	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void GetRtosHeapSize(void)
{
	u32 ex_heap_left = 0;
	u32 in_heap_left = 0;
	
	in_heap_left = xPortGetFreeHeapSize();
	ex_heap_left = xMyPortGetFreeHeapSize(SRAM_EX);
	
	SRV_DEBUGF(SRV_PRINTF_NOW, ("Heap left ram:%dk sram:%dk\r\n", in_heap_left/1024, ex_heap_left/1024));
}


/**
  * @brief  vTaskCheck.
  * @param  pvParameters: task parameters
  * @retval None
  */
void vTaskCheck(void *pvParameters)
{
	u8 cmd = 0;
	
    vSetupSysInfo();

    while(1)
    {
		if(xQueueReceive(xQueueUart, (void *)&cmd, portMAX_DELAY) == pdPASS)
		{
			switch(cmd)
			{
				case 0xA1:
					MEMP_STATS_DISPLAY(MEMP_TCPIP_MSG_INPKT); /* 统计MEMP的使用情况 */
					MEM_STATS_DISPLAY();  /* 统计MEM的使用情况 */
					SYS_STATS_DISPLAY();
					STATS_DISP_COM();	
					GetRtosHeapSize();
					break;
				case 0xA2:
					DispTaskInfo();
					break;
				default :
					SRV_DEBUGF(SRV_PRINTF_NOW, ("xQueueUart receive error\r\n"));
					break;
			}
		}		
    }
}


