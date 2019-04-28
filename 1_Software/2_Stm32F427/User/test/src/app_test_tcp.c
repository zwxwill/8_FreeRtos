/**
  ******************************************************************************
  * @file    app_test_tcp.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "app_test_tcp.h"
#include "test_tcp.h"

#include "FreeRTOS.h"
#include "task.h"

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
void vTaskTestTcpClient(void *pvParameters)
{
	test_tcp_client();
	
    while(1)
    {
        vTaskDelay(1000);
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestTcpServer(void *pvParameters)
{
	test_tcp_server();
	
    while(1)
    {
        vTaskDelay(1000);
    }
}

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


