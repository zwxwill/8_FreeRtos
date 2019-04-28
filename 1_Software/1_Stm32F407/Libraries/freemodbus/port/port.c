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



/* ----------------------- Variables ----------------------------------------*/
USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS];
USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

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
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
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
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
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




