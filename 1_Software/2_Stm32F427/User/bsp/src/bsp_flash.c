/**
  ******************************************************************************
  * @file    bsp_flash.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "bsp_flash.h"
#include <string.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_FLASH         SPI2
/* 片选GPIO端口  */
#define SF_CS_GPIO        GPIOB
#define SF_CS_PIN         GPIO_Pin_12
/* 片选口线置低选中  */
#define SF_CS_LOW()       SF_CS_GPIO->BSRRH = SF_CS_PIN
/* 片选口线置高不选中 */
#define SF_CS_HIGH()      SF_CS_GPIO->BSRRL = SF_CS_PIN
#define ENABLE_SPI_RCC()  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)
/*
    【SPI时钟最快是2分频，不支持不分频】
    如果是SPI1，2分频时SCK时钟 = 42M，4分频时SCK时钟 = 21M
    如果是SPI2, 2分频时SCK时钟 = 21M
*/
#define SPI_BAUD          SPI_BaudRatePrescaler_2


#define CMD_AAI       0xAD      /* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_DISWR     0x04      /* 禁止写, 退出AAI状态 */
#define CMD_EWRSR     0x50      /* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01      /* 写状态寄存器命令 */
#define CMD_WREN      0x06      /* 写使能命令 */
#define CMD_READ      0x03      /* 读数据区命令 */
#define CMD_RDSR      0x05      /* 读状态寄存器命令 */
#define CMD_RDID      0x9F      /* 读器件ID命令 */
#define CMD_SE        0x20      /* 擦除扇区命令 */
#define CMD_BE        0xC7      /* 批量擦除命令 */
#define CMD_PGWR      0x02      /* 写页 */

#define DUMMY_BYTE    0xA5      /* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG      0x01      /* 状态寄存器中的正在编程标志（WIP) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static SFLASH_T g_tSF;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  向器件发送一个字节，同时从MISO口线采样器件返回的数据
  * @param  None
  * @retval None
  */
static uint8_t bsp_FlashSendByte(uint8_t _ucValue)
{
    /* 等待上个数据未发送完毕 */
    while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_TXE) == RESET);

    /* 通过SPI硬件发送1个字节 */
    SPI_I2S_SendData(SPI_FLASH, _ucValue);

    /* 等待接收一个字节任务完成 */
    while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_RXNE) == RESET);

    /* 返回从SPI总线读到的数据 */
    return SPI_I2S_ReceiveData(SPI_FLASH);
}

/**
  * @brief
  * @param  None
  * @retval None
  */

static uint32_t bsp_FlashReadID(void)
{
    uint32_t uiID;
    uint8_t id1, id2, id3;

    SF_CS_LOW();                                          /* 使能片选 */
    bsp_FlashSendByte(CMD_RDID);                          /* 发送读ID命令 */
    id1 = bsp_FlashSendByte(DUMMY_BYTE);                  /* 读ID的第1个字节 */
    id2 = bsp_FlashSendByte(DUMMY_BYTE);                  /* 读ID的第2个字节 */
    id3 = bsp_FlashSendByte(DUMMY_BYTE);                  /* 读ID的第3个字节 */
    SF_CS_HIGH();                                         /* 禁能片选 */

    uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

    return uiID;
}

/**
  * @brief  读取器件ID,并填充器件参数
  * @param  None
  * @retval None
  */
static void bsp_ReadFlashInfo(void)
{
    g_tSF.ChipID = bsp_FlashReadID(); /* 芯片ID */

    switch(g_tSF.ChipID)
    {
        case SST25VF016B_ID:
            strcpy(g_tSF.ChipName, "SST25VF016B");
            g_tSF.TotalSize = 2 * 1024 * 1024;  /* 总容量 = 2M */
            g_tSF.PageSize = 4 * 1024;          /* 页面大小 = 4K */
            break;

        case MX25L1606E_ID:
            strcpy(g_tSF.ChipName, "MX25L1606E");
            g_tSF.TotalSize = 2 * 1024 * 1024;  /* 总容量 = 2M */
            g_tSF.PageSize = 4 * 1024;          /* 页面大小 = 4K */
            break;

        case W25Q64BV_ID:
            strcpy(g_tSF.ChipName, "W25Q64BV");
            g_tSF.TotalSize = 8 * 1024 * 1024;  /* 总容量 = 16M */
            g_tSF.PageSize = 4 * 1024;          /* 页面大小 = 4K */
            break;

        case W25Q128BV_ID:
            strcpy(g_tSF.ChipName, "W25Q128BV");
            g_tSF.TotalSize = 16 * 1024 * 1024; /* 总容量 = 8M */
            g_tSF.PageSize = 4 * 1024;          /* 页面大小 = 4K */
            break;

        default:
            strcpy(g_tSF.ChipName, "Unknow Flash");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            break;
    }
}

/**
  * @brief  向器件发送写使能命令
  * @param  None
  * @retval None
  */
static void bsp_FlashWriteEnable(void)
{
    SF_CS_LOW();                                    /* 使能片选 */
    bsp_FlashSendByte(CMD_WREN);                    /* 发送命令 */
    SF_CS_HIGH();                                   /* 禁能片选 */
}

/**
  * @brief  写状态寄存器
  * @param  None
  * @retval None
  */
static void bsp_FlashWriteStatus(uint8_t _ucValue)
{
    if(g_tSF.ChipID == SST25VF016B_ID)
    {
        /* 第1步：先使能写状态寄存器 */
        SF_CS_LOW();                                    /* 使能片选 */
        bsp_FlashSendByte(CMD_EWRSR);                   /* 发送命令， 允许写状态寄存器 */
        SF_CS_HIGH();                                   /* 禁能片选 */

        /* 第2步：再写状态寄存器 */
        SF_CS_LOW();                                    /* 使能片选 */
        bsp_FlashSendByte(CMD_WRSR);                    /* 发送命令， 写状态寄存器 */
        bsp_FlashSendByte(_ucValue);                    /* 发送数据：状态寄存器的值 */
        SF_CS_HIGH();                                   /* 禁能片选 */
    }
    else
    {
        SF_CS_LOW();                                    /* 使能片选 */
        bsp_FlashSendByte(CMD_WRSR);                    /* 发送命令， 写状态寄存器 */
        bsp_FlashSendByte(_ucValue);                    /* 发送数据：状态寄存器的值 */
        SF_CS_HIGH();                                   /* 禁能片选 */
    }
}

/**
  * @brief  采用循环查询的方式等待器件内部写操作完成
  * @param  None
  * @retval None
  */
static void bsp_FlashWaitForWriteEnd(void)
{
    SF_CS_LOW();                                        /* 使能片选 */
    bsp_FlashSendByte(CMD_RDSR);                        /* 发送命令， 读状态寄存器 */
    while((bsp_FlashSendByte(DUMMY_BYTE) & WIP_FLAG) == SET); /* 判断状态寄存器的忙标志位 */
    SF_CS_HIGH();                                       /* 禁能片选 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_InitSpiFlash(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* 使能GPIO 时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* 配置 SCK, MISO 、 MOSI 为复用功能 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI1);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置片选口线为推挽输出模式 */
    SF_CS_HIGH();       /* 片选置高，不选中 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /* 配置SPI硬件参数用于访问串行Flash */
    /* 打开SPI时钟 */
    ENABLE_SPI_RCC();

    /* 配置SPI硬件参数 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  /* 数据方向：2线全双工 */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       /* STM32的SPI工作模式 ：主机模式 */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   /* 数据位长度 ： 8位 */
    /* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
       本例配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)
    */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         /* 时钟上升沿采样数据 */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        /* 时钟的第2个边沿采样数据 */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;           /* 片选控制方式：软件控制 */

    /* 设置波特率预分频系数 */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BAUD;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  /* 数据位传输次序：高位先传 */
    SPI_InitStructure.SPI_CRCPolynomial = 7;            /* CRC多项式寄存器，复位后为7。本例程不用 */
    SPI_Init(SPI_FLASH, &SPI_InitStructure);

    SPI_Cmd(SPI_FLASH, DISABLE);            /* 先禁止SPI  */

    SPI_Cmd(SPI_FLASH, ENABLE);             /* 使能SPI  */

    bsp_ReadFlashInfo();              /* 自动识别芯片型号 */

    SF_CS_LOW();                /* 软件方式，使能串行Flash片选 */
    bsp_FlashSendByte(CMD_DISWR);     /* 发送禁止写入的命令,即使能软件写保护 */
    SF_CS_HIGH();               /* 软件方式，禁能串行Flash片选 */

    bsp_FlashWaitForWriteEnd();       /* 等待串行Flash内部操作完成 */

    bsp_FlashWriteStatus(0);          /* 解除所有BLOCK的写保护 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
uint8_t bsp_SpiReadWriteByte(uint8_t ucData)
{
    uint8_t ucReadData = 0;

    /* 等待发送缓冲区空 */
    while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_TXE) == RESET);

    /* 发送一个字节 */
    SPI_I2S_SendData(SPI_FLASH, ucData);

    /* 等待数据接收完毕 */
    while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_RXNE) == RESET);

    /* 读取接收到的数据 */
    ucReadData = SPI_I2S_ReceiveData(SPI_FLASH);

    return ucReadData;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_EraseSector(uint32_t _uiSectorAddr)
{
    bsp_FlashWriteEnable();                         /* 发送写使能命令 */

    /* 擦除扇区操作 */
    SF_CS_LOW();                                    /* 使能片选 */
    bsp_SpiReadWriteByte(CMD_SE);                                /* 发送擦除命令 */
    bsp_SpiReadWriteByte((_uiSectorAddr & 0xFF0000) >> 16);  /* 发送扇区地址的高8bit */
    bsp_SpiReadWriteByte((_uiSectorAddr & 0xFF00) >> 8);     /* 发送扇区地址中间8bit */
    bsp_SpiReadWriteByte(_uiSectorAddr & 0xFF);              /* 发送扇区地址低8bit */
    SF_CS_HIGH();                                   /* 禁能片选 */

    bsp_FlashWaitForWriteEnd();                     /* 等待串行Flash内部写操作完成 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_FlashRead(uint32_t sector_id, uint8_t *pData, uint32_t length)
{
    uint32_t addr = 0;

    /* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
    if((sector_id > SECTOR_NUM) ||(pData == (uint8_t *)0))
    {
        return;
    }
    addr = SECTOR_ADDR(sector_id);

    SF_CS_LOW();                                    /* 使能片选 */
    bsp_SpiReadWriteByte(CMD_READ);                         /* 发送读命令 */
    bsp_SpiReadWriteByte((addr & 0xFF0000) >> 16);  /* 发送扇区地址的高8bit */
    bsp_SpiReadWriteByte((addr & 0xFF00) >> 8);     /* 发送扇区地址中间8bit */
    bsp_SpiReadWriteByte(addr & 0xFF);              /* 发送扇区地址低8bit */
    while(length--)
    {
        *pData++ = bsp_SpiReadWriteByte(ERASE_BYTE); /* 读一个字节并存储到pBuf，读完后指针自加1 */
    }
    SF_CS_HIGH();                                   /* 禁能片选 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
int bsp_ClearSectors(int iSecorId, int length)
{
    int i = 0;
    int iSectorNum = 0;

    if((iSecorId > SECTOR_NUM) || (length < 0))
    {
        return FLASH_ERASE_ERROR;
    }

    /*Calculate the number of sectors needed*/
    iSectorNum = (uint32_t)length / SECTOR_SIZE;
    if(length % SECTOR_SIZE)
    {
        iSectorNum++;
    }

    /*Erase sectors before write data into*/
    for(i = 0; i < iSectorNum; i++)
    {
        bsp_EraseSector(SECTOR_ADDR(iSectorNum + i));
    }

    return FLASH_ERASE_OK;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_FlashPageWrite(uint8_t * buf, uint32_t addr, uint16_t length)
{
    uint32_t i = 0;

    bsp_FlashWriteEnable();
    SF_CS_LOW();
    bsp_SpiReadWriteByte(CMD_PGWR);
    bsp_SpiReadWriteByte((uint8_t)(addr >> 16));
    bsp_SpiReadWriteByte((uint8_t)(addr >> 8));
    bsp_SpiReadWriteByte((uint8_t)addr);
    for(i = 0; i < length; i++)
    {
        bsp_SpiReadWriteByte(buf[i]);
    }

    SF_CS_HIGH();
    bsp_FlashWaitForWriteEnd();
}
/**
  * @brief
  * @param  None
  * @retval None
  */
int bsp_FlashWrite(int sector_id, uint8_t *blob, int length)
{
    uint32_t num_sectors;
    uint32_t num_pages;
    uint32_t bytes_ramin;
    uint32_t addr;
    uint32_t i, j;

    if((sector_id >= SECTOR_NUM) || (blob == (uint8_t *)0) || (length < 0))
    {
        return FLASH_WRITE_ERROR;
    }

    /*Calculate the number of sectors needed*/
    num_sectors = (uint32_t)length / SECTOR_SIZE;
    if(length % SECTOR_SIZE)
    {
        num_sectors++;
    }

    /*Erase sectors before write data into*/
    for(i = 0; i < num_sectors; i++)
    {
        bsp_EraseSector(SECTOR_ADDR(sector_id + i));
    }
    /*Calculate the number of pages that will be fully used*/
    num_pages = (uint32_t)length / PAGE_SIZE;
    bytes_ramin = length % PAGE_SIZE;
    /*Write data into corresponding pages*/
    addr = SECTOR_ADDR(sector_id);//Calculate the start address
    for(j = 0; j < num_pages; j++)
    {
        bsp_FlashPageWrite(blob + j * PAGE_SIZE, addr + j * PAGE_SIZE, PAGE_SIZE);
    }
    if(bytes_ramin)
    {
        bsp_FlashPageWrite(blob + j * PAGE_SIZE, addr + j * PAGE_SIZE, bytes_ramin);
    }

    return FLASH_WRITE_OK;
}
















/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
