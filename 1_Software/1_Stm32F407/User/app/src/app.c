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
#include "bsp_lan8720.h"
#include "bsp_uart.h"
#include "bsp_ds3231.h"
#include "bsp_flash.h"
#include "bsp_rs485.h"
#include "bsp_sram.h"
#include "bsp_dm9k.h"

#include "app_led.h"
#include "app_check.h"
#include "app_tcpip.h"
#include "app_test.h"
#include "app_debug.h"

#include "bsp_dm9k.h"
#include "bsp_lan8720.h"
#include "bsp_ds3231.h"

#include "srv_netconf.h"
#include "srv_printf.h"

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
static TaskHandle_t xHandleTaskTCPIP = NULL;
static TaskHandle_t xHandleTaskTest = NULL;
static TaskHandle_t xHandleTaskDebug = NULL;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param  None
  * @retval None
  */
static void App_HardWareInit(void)
{
    bsp_InitLed();      /* 调试LED */
    bsp_InitUart();     /* 调试串口初始化  */
	
	
    bsp_InitDs3231();
    bsp_InitSram();     /* Sram初始化要放在前面，因为任务初始化，外部变量都要用到 */
    bsp_InitSpiFlash(); /* spi flash初始化 ： 这个要先初始化 便于加载配置信息 */

	Srv_RamPrintInit(); /* ram打印调试 放在SRAM初始化之后 */
    Srv_LwIPInit();     /* 以太网初始化 */
}


/**
  * @brief  数值越小，优先级越低
  * @param  None
  * @retval None
  */
static void App_TaskCreate(void)
{
    xTaskCreate(vTaskCheck,            /* function  */
                "vTaskCheck",          /* name    */
                256,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                1,                     /* priority */
                &xHandleTaskCheck);    /* handle  */

    xTaskCreate(vTaskDebug,             /* function  */
                "vTaskDebug",           /* name    */
                128,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                2,                     /* priority */
                &xHandleTaskDebug);     /* handle  */
	
    xTaskCreate(vTaskLED,              /* function  */
                "vTaskLED",            /* name    */
                128,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                3,                     /* priority */
                &xHandleTaskLED);      /* handle  */

    xTaskCreate(vTaskTCPIP,            /* function  */
                "vTaskTCPIP",          /* name    */
                128,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                4,                     /* priority */
                &xHandleTaskTCPIP);    /* handle  */

    /* JDI 队列接收使用了优先级5 */

    xTaskCreate(vTaskTest,             /* function  */
                "vTaskTest",           /* name    */
                512,                  /* stack size, unit: 4 byte */
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


