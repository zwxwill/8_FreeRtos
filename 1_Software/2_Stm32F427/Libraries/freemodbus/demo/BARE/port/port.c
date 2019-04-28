/*
 * FreeModbus Libary: LPC214X Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- System includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

/* ----------------------- other includes ----------------------------------*/
#include "stm32f4xx.h"
//extern xQueueHandle xMbSem;

/* ----------------------- Variables ----------------------------------------*/
#define MB_VAR_DATA_ADDR                 50000
#define MB_VAR_DATA_LOGIC_NUM            700

/*****************************************************************************
bit:
    read discrete coil       02
    read coil                01
    write single coil        05
16bit:
    read input register      04
    read multi register      03
    write single register    06
    write multi register     16


******************************************************************************/

/* ----------------------- Start implementation -----------------------------*/
void
EnterCriticalSection()
{
    __ASM volatile("cpsid i");
}

void
ExitCriticalSection()
{
    __ASM volatile("cpsie i");
}

void __aeabi_assert(const char * x1, const char * x2, int x3)
{

}

USHORT judgeAddr(USHORT usAddress)
{
    USHORT ret = 1;

//    if((usAddress >= MB_VAR_DATA_ADDR) && (usAddress <= MB_VAR_DATA_ADDR + (MB_VAR_DATA_LOGIC_NUM/2)))
//    {

//    }
//    else if((usAddress >= (4*200+1000)) && ((usAddress <= (31*200+1000))))
//    {

//    }
//    else if(usAddress <= 10000)
//    {
//
//    }
//    else
//    {
//        ret = 0;
//    }
    return ret;
}

USHORT getSlotAddr(USHORT usAddress)
{
//    if((usAddress >= MB_VAR_DATA_ADDR) && (usAddress <= MB_VAR_DATA_ADDR +  (MB_VAR_DATA_LOGIC_NUM/2)))
//    {
//        return MB_VAR_DATA_ADDR;
//    }
//    else if((usAddress >= (4*200+1000)) && ((usAddress <= (31*200+1000))))
//    {
//        return ((usAddress-1000)/200);
//    }
//    else
//    {
//        return 0;
//    }
}

USHORT getSlotStartAddr(USHORT slotAddr)
{
//    if(slotAddr == MB_VAR_DATA_ADDR)
//    {
//        return MB_VAR_DATA_ADDR;
//    }
//    else if(slotAddr == 0)
//    {
//        return 0;
//    }
//    else
//    {
//        return (slotAddr*200+1000);
//    }
}

/**
  *****************************************************************************
  * @Name   : Operation input register 16bit
  *
  * @Brief  : Corresponding function code 0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer£ºData buffer, used in response to the host
  *           usAddress:     Register address
  *           usNRegs:       Number of operation registers
  *
  * @Output : none
  *
  * @Return : Modbus status information
  *****************************************************************************
**/
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode    eStatus = MB_ENOERR;
//    int             iRegIndex;
//    USHORT          slotAddr;
//    uint16          *pData;
//    uint8           tmp = 0;

//    usAddress--;
//    if(judgeAddr(usAddress) != 1)
//    {
//        return MB_ENOREG;
//    }
//    slotAddr = getSlotAddr(usAddress);
//    if(slotAddr == MB_VAR_DATA_ADDR)
//    {
//        pData = (uint16 *)(Mb_VarData);
//    }
//    else
//    {
//        pData = (uint16 *)(MbData[slotAddr].pMb);
//    }

//    iRegIndex = (int)(usAddress - getSlotStartAddr(slotAddr));
//    while(usNRegs > 0)
//    {
//        *pucRegBuffer++ = (unsigned char)(pData[iRegIndex] >> 8);
//        *pucRegBuffer++ = (unsigned char)(pData[iRegIndex] & 0xFF);
//        iRegIndex++;
//        usNRegs--;
//    }

    return eStatus;
}

/**
  *****************************************************************************
  * @Name   : Operation hold register 16bit
  *
  * @Brief  : Corresponding function code
  *                     0x06 -> eMBFuncWriteHoldingRegister
  *                     0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                     0x03 -> eMBFuncReadHoldingRegister
  *                     0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer£ºData buffer, used in response to the host
  *           usAddress:     Register address
  *           usNRegs:       Number of operation registers
  *           eMode:         function code
  *
  * @Output : none
  *
  * @Return : Modbus status information
  *****************************************************************************
**/
eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
//    int          iRegIndex = 0;
//    USHORT          slotAddr;
//    uint16          *pData;
//    uint8           tmp = 0;

//    usAddress--;
//    if(judgeAddr(usAddress) != 1)
//    {
//        return MB_ENOREG;
//    }
//    slotAddr = getSlotAddr(usAddress);
//    if(slotAddr == MB_VAR_DATA_ADDR)
//    {
//        pData = (uint16 *)(Mb_VarData);
//    }
//    else
//    {
//        pData = (uint16 *)(MbData[slotAddr].pMb);
//    }
//

//    iRegIndex = (int)(usAddress - getSlotStartAddr(slotAddr));
//    switch(eMode)
//    {
//        case MB_REG_READ:
//            while(usNRegs > 0)
//            {
//                *pucRegBuffer++ = (uint8_t)(pData[iRegIndex] >> 8);
//                *pucRegBuffer++ = (uint8_t)(pData[iRegIndex] & 0xFF);
//                iRegIndex++;
//                usNRegs--;
//            }
//            break;

//        case MB_REG_WRITE:
//            if(slotAddr == MB_VAR_DATA_ADDR)
//            {
//                while(xQueueReceive(xMbSem, (void *)&tmp, portMAX_DELAY) != pdPASS);
//            }
//            while(usNRegs > 0)
//            {
//                pData[iRegIndex] = (uint16)((*pucRegBuffer) << 8);
//                pucRegBuffer++;
//                pData[iRegIndex] |= (uint16)(*pucRegBuffer);
//                pucRegBuffer++;
//                iRegIndex++;
//                usNRegs--;
//            }
//            if(slotAddr == MB_VAR_DATA_ADDR)
//            {
//                xQueueSend(xMbSem, (void *)(&tmp), 0U);
//            }
//            break;
//    }

    return eStatus;
}

/**
  *****************************************************************************
  * @Name   : Operate discrete registers  1bit
  *
  * @Brief  : Corresponding function code 0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer£ºData buffer, used in response to the host
  *           usAddress:     Register address
  *           usNRegs:       Number of operation registers
  *
  * @Output : none
  *
  * @Return : Modbus status information
  *****************************************************************************
**/
eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode    eStatus = MB_ENOERR;
//    int16_t         iNDiscrete = (int16_t)usNDiscrete;
//    uint16_t        usBitOffset;
//    USHORT          slotAddr;
//    uint8           *pData;
//    uint8           tmp = 0;

//    usAddress--;
//    if(judgeAddr(usAddress) != 1)
//    {
//        return MB_ENOREG;
//    }
//    slotAddr = getSlotAddr(usAddress);
//    if(slotAddr == MB_VAR_DATA_ADDR)
//    {
//        pData = (uint8 *)(Mb_VarData);
//    }
//    else
//    {
//        pData = (uint8 *)(MbData[slotAddr].pMb);
//    }

//    usBitOffset = (uint16_t)(usAddress - getSlotStartAddr(slotAddr));

//    while(iNDiscrete > 0)
//    {
//        *pucRegBuffer++ = xMBUtilGetBits(pData, usBitOffset,
//                                         (uint8_t)(iNDiscrete > 8 ? 8 : iNDiscrete));
//        iNDiscrete -= 8;
//        usBitOffset += 8;
//    }

    return eStatus;
}

/**
  *****************************************************************************
  * @Name   : Operating coil
  *
  * @Brief  : Corresponding function code
  *                    0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> Write multiple coils eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer£ºData buffer, used in response to the host
  *           usAddress:     Register address
  *           usNRegs:       Number of operation registers
  *           eMode:         function code
  *
  * @Output : none
  *
  * @Return : Modbus status information
  *****************************************************************************
**/
eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
//    int16_t         iNCoils = (int16_t)usNCoils;
//    int16_t         usBitOffset;
//    USHORT          slotAddr;
//    uint8           *pData;
//    uint8           tmp = 0;

//    usAddress--;
//    if(judgeAddr(usAddress) != 1)
//    {
//        return MB_ENOREG;
//    }
//    slotAddr = getSlotAddr(usAddress);
//    if(slotAddr == MB_VAR_DATA_ADDR)
//    {
//        pData = (uint8 *)(Mb_VarData);
//    }
//    else
//    {
//        pData = (uint8 *)(MbData[slotAddr].pMb);
//    }

//    usBitOffset = (uint16_t)(usAddress - getSlotStartAddr(slotAddr));
//    switch(eMode)
//    {
//        case MB_REG_READ:
//            while(iNCoils > 0)
//            {
//                *pucRegBuffer++ = xMBUtilGetBits(pData, usBitOffset,
//                                                 (uint8_t)(iNCoils > 8 ? 8 : iNCoils));
//                iNCoils -= 8;
//                usBitOffset += 8;
//            }
//            break;

//        case MB_REG_WRITE:
//            if(slotAddr == MB_VAR_DATA_ADDR)
//            {
//                while(xQueueReceive(xMbSem, (void *)&tmp, portMAX_DELAY) != pdPASS);
//            }
//            while(iNCoils > 0)
//            {
//                xMBUtilSetBits(pData, usBitOffset,
//                               (uint8_t)(iNCoils > 8 ? 8 : iNCoils),
//                               *pucRegBuffer++);
//                iNCoils -= 8;
//            }
//            if(slotAddr == MB_VAR_DATA_ADDR)
//            {
//                xQueueSend(xMbSem, (void *)(&tmp), 0U);
//            }
//            break;
//    }

    return eStatus;
}




