/**
  ******************************************************************************
  * @file    app.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "app.h"
#include "FreeRTOS.h"
#include "task.h"

#include "bsp_led.h"
#include "bsp_uart.h"

#include "app_led.h"
#include "app_check.h"
#include "app_test.h"

#include <stdio.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xHandleTaskStart = NULL;
static TaskHandle_t xHandleTaskCheck = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskTest = NULL;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param  None
  * @retval None
  */
static void App_HardWareInit(void)
{
    bsp_InitLed();      /* 调试LED */
//    bsp_InitUart();     /* 调试串口初始化  */

}


/**
  * @brief  数值越小，优先级越低
  * @param  None
  * @retval None
  */
static void App_TaskCreate(void)
{
//    xTaskCreate(vTaskCheck,            /* function  */
//                "vTaskCheck",          /* name    */
//                1024,                  /* stack size, unit: 4 byte */
//                NULL,                  /* task param */
//                2,                     /* priority */
//                &xHandleTaskCheck);    /* handle  */
	
    xTaskCreate(vTaskLED,              /* function  */
                "vTaskLED",            /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                3,                     /* priority */
                &xHandleTaskLED);      /* handle  */


    /* JDI 队列接收使用了优先级5 */

    xTaskCreate(vTaskTest,             /* function  */
                "vTaskTest",           /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                8,                     /* priority */
                &xHandleTaskTest);     /* handle  */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
static void vTaskStart(void *pvParameters)
{
    App_HardWareInit();

    App_TaskCreate();

    vTaskDelete(NULL);
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void app_Init(void)
{
    xTaskCreate(vTaskStart,            /* function  */
                "vTaskStart",          /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                configMAX_PRIORITIES-1,/* priority */
                &xHandleTaskStart);    /* handle  */
}


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


