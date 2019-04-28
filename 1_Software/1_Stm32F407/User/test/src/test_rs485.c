/**
  ******************************************************************************
  * @file    test_rs485.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_rs485.h"
#include "bsp_rs485.h"

#include "FreeRtos.h"
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
void vTaskTestRs485(void *pvParameters)
{
	T_UartCfg cfg;
	u8 buf[3] = {1, 2, 3};
	
	cfg.mode = PLC_UART_485;
	cfg.baud = PLC_BAUDRATE_115200;
	cfg.parity = PLC_PARITY_NO;
	cfg.databit = PLC_DATABIT_8;
	cfg.stopbit = PLC_STOPBIT_1;
	
	Bsp_ConfigRs485(PLC_STM32_UART_COM1, &cfg);
	Bsp_ConfigRs485(PLC_STM32_UART_COM2, &cfg);
	Bsp_ConfigRs485(PLC_STM32_UART_COM3, &cfg);
	
    while(1)
    {
		Rs485_SendData(PLC_COM1, buf, 3);
		Rs485_SendData(PLC_COM2, buf, 3);
		Rs485_SendData(PLC_COM3, buf, 3);
		
        vTaskDelay(1000);
    }	
}  

  


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


