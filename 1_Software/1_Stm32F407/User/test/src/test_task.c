/**
  ******************************************************************************
  * @file    test_task.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* 经测试发现时可以启动两个函数相同，但任务名不同的任务 */

/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestTask(void *pvParameters)
{
	static int i = 0;
	int pori = 0;
	
    while(1)
    {
		pori = uxTaskPriorityGet(0);
		if(pori == 9)
		{
			printf("Task1 = %d\r\n", i++);
		}
		if(pori == 10)
		{
			printf("Task2 = %d\r\n", i++);
		}
		
        vTaskDelay(1000);
    }
}

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


