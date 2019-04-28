/**
  ******************************************************************************
  * @file    app_debug.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "app_debug.h"
#include "srv_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lwip/stats.h"
#include "arch/sys_arch.h"

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/


/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskDebug(void *pvParameters)
{
	u8 cmd = 0;
	
	while(1)
	{
		Srv_RamPrintShow();	
		vTaskDelay(5);
	}
}



/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
