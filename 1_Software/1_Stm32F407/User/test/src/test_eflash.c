/**
  ******************************************************************************
  * @file    test_eflash.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_eflash.h"
#include "elog_flash.h"
#include "easyflash.h"
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

static void elog_user_assert_hook(const char* ex, const char* func, size_t line) {

#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    /* disable async output */
    elog_async_enabled(false);
#endif

    /* disable logger output lock */
    elog_output_lock_enabled(false);
    /* disable flash plugin lock */
    elog_flash_lock_enabled(false);
    /* output logger assert information */
    elog_a("elog", "(%s) has assert failed at %s:%ld.\n", ex, func, line);
    /* write all buffered log to flash */
    elog_flash_flush();
    while(1);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void TestEFlashInit(void)
{
    /* initialize EasyLogger */
    elog_init();
	/* initialize EasyFlash */
	easyflash_init();
	
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
	/* set EasyLogger assert hook */
	elog_assert_set_hook(elog_user_assert_hook);
	
	/* initialize EasyLogger Flash plugin */
	elog_flash_init();
    /* start EasyLogger */
    elog_start();
	

}



/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestEFlash(void *pvParameters)
{
	static int cnt = 0;
	char messagelog[] = "This is log message!\n";
	char messageflashlog[] = "This is flash log message!\n";
	TestEFlashInit();
	
	vTaskDelay(1000);
	
//	elog_flash_clean();
	elog_flash_output_all(); /* output all saved log from flash */
	
    while(1)
    {
		cnt++;
		if(cnt == 10)
		{
			cnt = 0;
			elog_flash_flush();
		}
		
		log_d(messagelog);

		elog_flash_write(messageflashlog, sizeof(messageflashlog));
		
//		elog_raw("Hello EasyLogger!\n");
//		/* trigger assert. Now will run elog_user_assert_hook. All log information will save to flash. */
//		ELOG_ASSERT(0);	
		
        vTaskDelay(1000);
    }
}












/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


