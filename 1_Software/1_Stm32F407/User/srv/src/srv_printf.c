/**
  ******************************************************************************
  * @file    srv_ramprint.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "srv_printf.h"
#include "heap_6.h"
#include "bsp_uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <stdarg.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PRINT_BUF_SIZE     (5*1024)
/* Private variables ---------------------------------------------------------*/
static u8 *pPrintBuf;
static u32 g_uiRamReadPos = 0;
static u32 g_uiRamNextPos = 0;
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  
  * @param  None
  * @retval None
  */
static u32 ConvertU32ToStr(u32 val, char *pStr, u8 size, u8 base)
{
	u8   u8Temp = 0u;
	char acTemp[12] = {0};
	u8   u8WriteChar = 0u;
	u8   u8Digit = 0u;
	u32  ret = 0u;
	
	if((size == 0u) || (pStr == (void*)0) || ((base != 10u) && (base != 16u)))
	{
		
	}
	else
	{
        if(val == 0u)
        {
            acTemp[u8Temp++] = '0';
        }

        if(base == 10u) /* decimal */
        {
            while(val != 0u)
            {
                acTemp[u8Temp++] = (char)('0' + (val % 10u));
                val /= 10u;
            }

            while((u8Temp-- != 0u) && (u8WriteChar < (size-1u)))
            {
                /* write converted str into buffer from msb to lsb */
                pStr[u8WriteChar++] = acTemp[u8Temp];
            }
        }
		else
        {
            /* hex */      
            while(val != 0u)
            {
                u8Digit = (u8)val % 16u;
                if(u8Digit < 10u)
                {
                    acTemp[u8Temp++] = (char)('0' + u8Digit);
                }
                else
                {
                    acTemp[u8Temp++] = (char)('A' + u8Digit - 10u);
                }
                val /= 16u;
            }

            pStr[0] = '0';
            pStr[1] = 'x';
            u8WriteChar += 2u;
            while((u8Temp-- != 0u) && (u8WriteChar < (size-1u)))
            {
                /* write converted str into buffer from msb to lsb */
                pStr[u8WriteChar++] = acTemp[u8Temp];
            }
        }
		
		pStr[u8WriteChar] = '\0';
		
        /*
         * Returns the length of the string without null terminated character.
         */    
        ret = (u32)u8WriteChar;		
	}
	return ret;
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void RamPrintStr(char *pStr)
{
	u32 i = 0;
	
	if(pStr != (char *)0)
	{
		while(pStr[i] != '\0')
		{			
			pPrintBuf[g_uiRamNextPos % PRINT_BUF_SIZE] = pStr[i];
			g_uiRamNextPos++;
			i++;
		}
	}
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void RamPrintVal(u32 val, u8 base)
{
	u32 i = 0;
	char temp[12] = {'\0'};

	ConvertU32ToStr(val, temp, 12u, base);
	while(temp[i] != '\0')
	{		
		pPrintBuf[g_uiRamNextPos % PRINT_BUF_SIZE] = temp[i];	
		g_uiRamNextPos++;
		i++;
	}
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_RamPrintInit(void)
{
	pPrintBuf = pvMyPortMalloc(PRINT_BUF_SIZE, SRAM_EX);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_RamPrintShow(void)
{
	uint32_t StartTime=0, EndTime=0, Elapsed=0;
	uint32_t curPos=0;
	
	StartTime = xTaskGetTickCount();
	curPos = g_uiRamNextPos;
	
	while(((curPos - g_uiRamReadPos) != 0) && (Elapsed < 100))
	{
		Uart_SendByte(pPrintBuf[g_uiRamReadPos % PRINT_BUF_SIZE]);
		g_uiRamReadPos++;
		curPos = g_uiRamNextPos;			
		EndTime = xTaskGetTickCount();
		Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;	
	}
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_RamPrintf(const char *format, ...)
{
	char buf[128] = {0};
	va_list ap;
	
	va_start(ap, format);
	if(vsprintf(buf, format, ap) > 0)
	{
		RamPrintStr(buf);
	}		
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_UartPrintf(const char *format, ...)
{
	char buf[128] = {0};
	va_list ap;
	
	va_start(ap, format);
	if(vsprintf(buf, format, ap) > 0)
	{
		Uart_Printf(buf);
	}			
}



/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


