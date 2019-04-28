/**
  ******************************************************************************
  * @file    app_test_tcp.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#ifndef __APP_TEST_TCP_H__
#define __APP_TEST_TCP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported define -----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported struct  ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void vTaskTestTcpClient(void *pvParameters);
extern void vTaskTestTcpServer(void *pvParameters);
	
	
	
#ifdef __cplusplus
}
#endif

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

