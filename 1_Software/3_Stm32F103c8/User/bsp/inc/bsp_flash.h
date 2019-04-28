/**
  ******************************************************************************
  * @file    bsp_flash.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stm32f4xx.h"


/* Exported define -----------------------------------------------------------*/
#define FLASH_READ_OK                   0
#define FALSH_READ_ERROR                1
#define FLASH_ERASE_OK                  0
#define FLASH_ERASE_ERROR               2
#define FLASH_WRITE_OK                  0
#define FLASH_WRITE_ERROR               3
/* W25Q128FVSIG 规格说明 */
#define ERASE_BYTE                      0xff
#define PAGE_SIZE                       256
#define SECTOR_SIZE                     (16 * PAGE_SIZE) //4K byte
#define BLOCK_SIZE                      (16 * SECTOR_SIZE) //64K byte
#define BLOCK_NUM                       256
#define SECTOR_NUM                      (16 * BLOCK_NUM)//one block contains 16 sectors


/*
*********************************************************************************************************
*   Calculate the start address of the id'th sector
*********************************************************************************************************
*/
#define SECTOR_ADDR(id)                 ((id) * SECTOR_SIZE)//one sector == 4KB

/* Exported typedef ----------------------------------------------------------*/
/* 定义串行Flash ID */
enum
{
    SST25VF016B_ID = 0xBF2541,
    MX25L1606E_ID  = 0xC22015,
    W25Q64BV_ID    = 0xEF4017,
    W25Q128BV_ID   = 0xEF4018
};

typedef struct
{
    uint32_t ChipID;        /* 芯片ID */
    char ChipName[16];      /* 芯片型号字符串，主要用于显示 */
    uint32_t TotalSize;     /* 总容量 */
    uint16_t PageSize;      /* 页面大小 */
} SFLASH_T;

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void bsp_InitSpiFlash(void);
void bsp_FlashRead(uint32_t sector_id, uint8_t *pData, uint32_t length);
int bsp_ClearSectors(int iSecorId, int length);
int bsp_FlashWrite(int sector_id, uint8_t *blob, int length);

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
