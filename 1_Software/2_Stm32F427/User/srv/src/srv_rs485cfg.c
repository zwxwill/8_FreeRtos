/**
  ******************************************************************************
  * @file    srv_rs485cfg.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "srv_rs485cfg.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static T_UartCfg g_tUartCfg;
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  init drac200m config
  * @param  None
  * @retval None
  */
////void Srv_Rs485ParamTransport(T_Drac200mCommCfg *ptCommCfg, T_UartCfg *ptUartCfg)
//{
//    int i = 0;

//    for(i=0; i<DRAC200M_UART_NUM; i++)
//    {
//        ptUartCfg->mode = PLC_UART_485;

//        switch(ptCommCfg->uart[i].baud)
//        {
//            case 115200:
//                ptUartCfg->baud = PLC_BAUDRATE_115200;
//                break;
//            case 9600:
//                ptUartCfg->baud = PLC_BAUDRATE_9600;
//                break;
//            case 38400:
//                ptUartCfg->baud = PLC_BAUDRATE_38400;
//                break;
//            case 300:
//                ptUartCfg->baud = PLC_BAUDRATE_300;
//                break;
//        }

//        switch(ptCommCfg->uart[i].databit)
//        {
//            case 8:
//                ptUartCfg->databit = PLC_DATABIT_8;
//                break;
//        }

//        switch(ptCommCfg->uart[i].stopbit)
//        {
//            case 1:
//                ptUartCfg->stopbit = PLC_STOPBIT_1;
//                break;
//        }

//        switch(ptCommCfg->uart[i].parity)
//        {
//            case 0:
//                ptUartCfg->parity = PLC_PARITY_NO;
//                break;
//            case 1:
//                ptUartCfg->parity = PLC_PARITY_ODD;
//                break;
//            case 2:
//                ptUartCfg->parity = PLC_PARITY_EVEN;
//                break;
////        }
////    }
//}

/**
  * @brief
  * @param  None
  * @retval None
  */
int Srv_InitRs485(void)
{
//    int ret = 0;
//    int i = 0;
//    T_Drac200mCommCfg *ptCommCfg;

//    ptCommCfg = Plc_GetCommCfg();
//    Srv_Rs485ParamTransport(ptCommCfg, &g_tUartCfg);

//    for(i=0; i<DRAC200M_UART_NUM; i++)
//    {
//        ret = Bsp_ConfigRs485(i, &g_tUartCfg);
//        if(ret != UART_OK)
//        {
//            return ret;
//        }
//    }
//    return ret;
}


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
