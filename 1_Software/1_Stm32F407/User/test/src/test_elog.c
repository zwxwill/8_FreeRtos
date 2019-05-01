/**
  ******************************************************************************
  * @file    test_elog.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  
#include "test_elog.h"
#include "elog.h"

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

void vTaskTestA(void *pvParameters)
{
	while(1)
	{
		log_d("TaskA is now running!");
        vTaskDelay(4);
    }
}
void vTaskTestB(void *pvParameters)
{
	while(1)
    {
		log_d("TaskB not let A run!");
        vTaskDelay(5);
    }
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void TestElogInit(void)
{
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME | ELOG_FMT_FUNC);
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_TIME | ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();
	
    xTaskCreate(vTaskTestA,         /* function  */
                "vTaskTestElog",       /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                9,                     /* priority */
                NULL);                 /* handle  */
				
    xTaskCreate(vTaskTestB,         /* function  */
                "vTaskTestElog",       /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                9,                     /* priority */
                NULL);                 /* handle  */
	
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestElog(void *pvParameters)
{
	TestElogInit();
    while(1)
    {
		log_d("This is assert!");
//		printf("This is assert\n");
//		log_e("Hello EasyLogger!\n");
//		log_w("Hello EasyLogger!\n");
//		log_i("Hello EasyLogger!\n");
//		log_d("Hello EasyLogger!\n");
//		log_v("Hello EasyLogger!\n");
	//	printf("-----\r\n");
        vTaskDelay(1000);
    }
}





/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

