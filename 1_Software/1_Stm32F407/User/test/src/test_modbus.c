/**
  ******************************************************************************
  * @file    test_modbus.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_modbus.h"
#include "mb.h"
#include "port.h"

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
void vTaskTestModbusTcp(void *pvParameters)
{
	test_modbus_tcp();
	
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
void test_modbus_tcp(void)
{
	eMBErrorCode    xStatus;
	portTickType    xTicks;
	
	if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
	{
		vMBPortLog( MB_LOG_ERROR, "PPP", "can't initalize modbus stack!\r\n" );
	}
	else if( eMBEnable(  ) != MB_ENOERR )
	{
		vMBPortLog( MB_LOG_ERROR, "PPP", "can't enable modbus stack!\r\n" );
	}
	else
	{
		do
		{
			/* Application code here. */
			xStatus = eMBPoll(  );

			/* Update input registers with the current system  tick. */
			xTicks = xTaskGetTickCount(  );

			/* Note: little endian stuff */
			usRegInputBuf[0] = ( USHORT ) ( xTicks );
			usRegInputBuf[1] = ( USHORT ) ( xTicks >> 16UL );
		}
		while(  xStatus == MB_ENOERR );

		( void )eMBDisable(  );
		( void )eMBClose(  );
		printf("error\r\n");
	}
}	




/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/



