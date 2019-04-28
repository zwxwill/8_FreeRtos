/**
  ******************************************************************************
  * @file    srv_printf.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#ifndef __SRV_PRINT_H__
#define __SRV_PRINT_H__

#include "stm32f4xx.h"

/* Exported define -----------------------------------------------------------*/
#define SRV_DBG_LEVEL_ALL     0x00
#define SRV_DBG_LEVEL_OFF     SRV_DBG_LEVEL_ALL /* compatibility define only */
#define SRV_DBG_LEVEL_DEBUG   0x01 /* bad checksums, dropped packets, ... */
#define SRV_DBG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define SRV_DBG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define SRV_DBG_LEVEL_SEVERE  0x03
#define SRV_DBG_MASK_LEVEL    0x03
#define SRV_DBG_MIN_LEVEL     SRV_DBG_LEVEL_SEVERE 

/** flag for SRV_DEBUGF to enable that debug message */
#define SRV_DBG_ON            0x80U
/** flag for SRV_DEBUGF to disable that debug message */
#define SRV_DBG_OFF           0x00U

/** flag for SRV_DEBUGF to enable that debug message */
#define SRV_DBG_RAM            0x00U
/** flag for SRV_DEBUGF to disable that debug message */
#define SRV_DBG_UART           0x40U

#define SRV_DBG_HALT          0x20U

#define SRV_PLATFORM_DIAG(x) do {Srv_RamPrintf x;} while(0)
#define SRV_PRINTF_UART(x) do {Srv_UartPrintf x;} while(0)

#define SRV_DEBUGF(debug, message) do { \
                               if ( \
                                   ((debug) & SRV_DBG_ON) && \
                                   ((s16)((debug) & SRV_DBG_MASK_LEVEL) >= SRV_DBG_MIN_LEVEL)) { \
                                 SRV_PLATFORM_DIAG(message); \
                                    if ((debug) & SRV_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

#define SRV_PRINTF(debug, message) do { \
                               if ( \
                                   ((debug) & SRV_DBG_ON) && \
                                   ((s16)((debug) & SRV_DBG_MASK_LEVEL) >= SRV_DBG_MIN_LEVEL)) { \
                                 SRV_PRINTF_UART(message); \
                                 if ((debug) & SRV_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)							 

/****************   USER defined debug option ************************/
//#define  SRV_TEST_DEBUG            	SRV_DBG_ON						 
//SRV_DEBUGF(SRV_TEST_DEBUG | SRV_DBG_LEVEL_SEVERE, ("hello this is SRV_DEBUGF %d \r\n", 123));
//SRV_PRINTF(SRV_TEST_DEBUG | SRV_DBG_LEVEL_SEVERE, ("hello this is SRV_PRINTF %d \r\n", 789));

#define SRV_PRINTF_NOW             (SRV_DBG_ON|SRV_DBG_LEVEL_SEVERE)
#define SRV_HEAP_DEBUG            	SRV_DBG_ON	 // FreeRtos 内存
							 
#define SRV_MQTT_DEBUG              SRV_DBG_ON 
							 
							 
/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void RamPrintVal(u32 val, u8 base);
void RamPrintStr(char *pStr);
void Srv_RamPrintShow(void);
void Srv_RamPrintInit(void);
void RamPrintByte(u8 ch);
void Srv_RamPrintf(const char *format, ...);
void Srv_UartPrintf(const char *format, ...);

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

