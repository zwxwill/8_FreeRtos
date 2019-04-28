/**
  ******************************************************************************
  * @file    app_test.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "app_test.h"
#include "app_test_tcp.h"

#include "FreeRTOS.h"
#include "task.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xHandleTestTcpClient = NULL;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  vTaskHost
  * @param  None
  * @retval None
  */
void vTaskTest(void *pvParameters)
{
    xTaskCreate(vTaskTestTcpClient,    /* function  */
                "vTaskTestTcpClient",  /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                9,                     /* priority */
                &xHandleTestTcpClient);/* handle  */
	
    while(1)
    {
        vTaskDelay(2);
    }
}



/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


