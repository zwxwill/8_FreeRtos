/**
  ******************************************************************************
  * @file    app_led.c
  * @author  zwx
  * @version V1.0.0
  * @date    01/20/2019
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_led.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "app_debug.h"
#include "srv_printf.h"

#include "bsp_led.h"



/* Export variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  vTaskLED
  * @param  None
  * @retval None
  */
void vTaskLED(void *pvParameters)
{
    while(1)
    {
        bsp_LedToggle(1);
        vTaskDelay(4000);
    }
}


