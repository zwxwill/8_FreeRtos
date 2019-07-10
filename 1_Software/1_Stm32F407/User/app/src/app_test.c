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
#include "heap_6.h"
#include "test_tcp.h"
#include "test_modbus.h"
#include "test_stackoverflow.h"
#include "test_udp.h"
#include "test_netio.h"
#include "test_mqtt.h"
#include "test_rs485.h"
#include "test_task.h"
#include "test_elog.h"
#include "test_eflash.h"
#include "test_rawtcpclient.h"
#include "test_paho-mqtt.h"

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

u8 *pTest;

/**
  * @brief  vTaskHost
  * @param  None
  * @retval None
  */
void vTaskTest(void *pvParameters)
{
//	Test_StackOverFlow();
	
    xTaskCreate(vTaskTestTcpClient,    /* function  */
                "vTaskTestTcpClient",  /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                9,                     /* priority */
                NULL);                 /* handle  */
	
//    xTaskCreate(vTaskTestTcpServer,    /* function  */
//                "vTaskTestTcpServer",  /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */
	
//    xTaskCreate(vTaskTestModbusTcp,    /* function  */
//                "vTaskTestModbusTcp",  /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */	
//	configASSERT(0);

//    xTaskCreate(vTaskTestUdpServer,    /* function  */
//                "vTaskTestUdpServer",  /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(vTaskTestNetio,        /* function  */
//                "vTaskTestNetio",      /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(vTaskTestMqtt,         /* function  */
//                "vTaskTestMqtt",       /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(Test_PahoMqtt,         /* function  */
//                "Test_PahoMqtt",       /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */			

//    xTaskCreate(vTaskTestElog,         /* function  */
//                "vTaskTestElog",       /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */
				
//    xTaskCreate(vTaskTestEFlash,         /* function  */
//                "vTaskTestEFlash",       /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(vTaskTestRawTcpClient,  /* function  */
//                "vTaskTestRawTcpClient",/* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(vTaskTestRs485,         /* function  */
//                "vTaskTestRs485",       /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */

//    xTaskCreate(vTaskTestTask,         /* function  */
//                "vTaskTestTask1",      /* name    */
//                256,                   /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                9,                     /* priority */
//                NULL);                 /* handle  */
//				
//    xTaskCreate(vTaskTestTask,         /* function  */
//                "vTaskTestTask2",      /* name    */
//                256,                   /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                10,                    /* priority */
//                NULL);                 /* handle  */				

    while(1)
    {
        vTaskDelay(2);
    }
}



/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


