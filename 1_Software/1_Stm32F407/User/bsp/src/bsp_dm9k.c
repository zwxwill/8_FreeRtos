#include "bsp_dm9k.h"
#include "lwip/netif.h"
#include "bsp.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include <stddef.h>

/* Exported variables --------------------------------------------------------*/
extern xSemaphoreHandle s_xInputSemaphore;
extern xSemaphoreHandle s_xLockSemaphore;
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define printk(...)
#define printk printf
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
*   函数名: DM9K_GPIOConfig
*   参  数: 无
*   返  回: 无
*   功  能: 配置DM9000AE控制口线，FSMC管脚设置为复用功能
*/
void DM9K_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

    /* 设置 PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) 为复用推挽输出 */

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* 设置 PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) 为复用推挽输出 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* 设置 PE.4(CMD)  为复用推挽输出 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* 设置 PG12 (CS)) 为复用推挽输出 */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*******************************************************************************
*   函数名: DM9K_NVICConfig
*   参  数: 无
*   返  回: 无
*   功  能: 配置DM9000AE控制口线，FSMC管脚设置为复用功能
*/
void DM9K_NVICConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能 GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* 使能SYSFG时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* PC0 是DM9000_INT中断输入口 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //PC0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOC,&GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_ClearITPendingBit(EXTI_Line0);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;                        //外部中断线0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI0_IRQ_PRI;   //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                      //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*******************************************************************************
*   函数名: DM9K_FSMCConfig
*   参  数: 无
*   返  回: 无
*   功  能: 配置FSMC并口访问时序
*/
void DM9K_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p;

    /* 使能FSMC时钟 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 3 ----------------------------------------*/
    /*-- FSMC Configuration ------------------------------------------------------*/
    p.FSMC_AddressSetupTime = 5;        /* 设置为2会出错; 3正常 */
    p.FSMC_AddressHoldTime = 1;
    p.FSMC_DataSetupTime = 10;          /* 设置为1出错，2正常 */
    p.FSMC_BusTurnAroundDuration = 5;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;   // FSMC_MemoryType_PSRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /*!< Enable FSMC Bank1_SRAM3 Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

/*******************************************************************************
*   函数名: dm9k_udelay
*   参  数: time ： 延迟时间，不精确，us级别
*   返  回: 无
*   功  能: 延迟函数
*/
void dm9k_udelay(uint16_t time)
{
    uint16_t i,k;

    for(i = 0; i < time; i++)
    {
        for(k = 0; k < 100; k++);
    }
    while(time--);
}

/*
*********************************************************************************************************
*   函 数 名: dm9k_ReadReg
*   功能说明: 读出DM9000指定寄存器的值
*   形    参: reg 寄存器地址
*   返 回 值: 寄存器值
*********************************************************************************************************
*/
uint8_t dm9k_ReadReg(uint8_t reg)
{
    NET_REG_ADDR = reg;
    return (uint8_t)(NET_REG_DATA);
}

/*
*********************************************************************************************************
*   函 数 名: dm9k_WriteReg
*   功能说明: 读出DM9000指定寄存器的值
*   形    参: reg ：寄存器地址
*            writedata : 写入的数据
*   返 回 值: 无
*********************************************************************************************************
*/
void dm9k_WriteReg(uint8_t reg, uint8_t writedata)
{
    NET_REG_ADDR = reg;
    NET_REG_DATA = writedata;
}

/*******************************************************************************
*   函数名: dm9k_phy_write
*   参  数: phy_reg ： 寄存器地址
*           writedata ： 写入的数据
*   返  回: 无
*   功  能: 写DM9000A PHY 寄存器
*/
void dm9k_phy_write(uint8_t phy_reg, uint16_t writedata)
{
    /* 设置写入 PHY 寄存器的位置 */
    dm9k_WriteReg(DM9000_REG_EPAR, phy_reg | DM9000_PHY);

    /* 设置写入 PHY 寄存器的值 */
    dm9k_WriteReg(DM9000_REG_EPDRH, (writedata >> 8) & 0xff);
    dm9k_WriteReg(DM9000_REG_EPDRL, writedata & 0xff);

    dm9k_WriteReg(DM9000_REG_EPCR, 0x0a);                       /* 将资料写入 PHY 寄存器 */
    while(dm9k_ReadReg(DM9000_REG_EPCR) & 0x01);                /* 查寻是否执行结束 */
    dm9k_udelay(500);
    dm9k_WriteReg(DM9000_REG_EPCR, 0x00);                       /* 清除写入命令 */
}

/*******************************************************************************
*   函数名: dm9k_phy_read
*   参  数: phy_reg ： 寄存器地址
*   返  回: 无
*   功  能: 读DM9000A PHY 寄存器
*/
u16 dm9k_phy_read(uint8_t phy_reg)
{
    u16 temp = 0;

    /* 设置写入 PHY 寄存器的位置 */
    dm9k_WriteReg(DM9000_REG_EPAR, phy_reg | DM9000_PHY);

    dm9k_WriteReg(DM9000_REG_EPCR, 0x0C);                       /* 将资料读入 PHY 寄存器 */
    while(dm9k_ReadReg(DM9000_REG_EPCR) & 0x01);                /* 查寻是否执行结束 */
    dm9k_udelay(500);
    dm9k_WriteReg(DM9000_REG_EPCR, 0x00);                       /* 清除写入命令 */

    temp=(dm9k_ReadReg(DM9000_REG_EPDRH)<<8)|(dm9k_ReadReg(DM9000_REG_EPDRL));

    return temp;
}

/* Set PHY operationg mode */
void dm9k_phy_mode_set(uint32_t media_mode)
{
    uint16_t phy_reg4 = 0, phy_reg0 = 0;

    switch(media_mode)
    {
        case DM9000_10MHD:
            phy_reg4 = 0x21;
            phy_reg0 = 0x0000;
            break;
        case DM9000_10MFD:
            phy_reg4 = 0x41;
            phy_reg0 = 0x1100;
            break;
        case DM9000_100MHD:
            phy_reg4 = 0x81;
            phy_reg0 = 0x2000;
            break;
        case DM9000_100MFD:
            phy_reg4 = 0x101;
            phy_reg0 = 0x3100;
            break;
        default:
            phy_reg4 = 0x01e1;
            phy_reg0 = 0x1000;
            break;
    }
    dm9k_WriteReg(DM9000_REG_GPR, DM9000_PHY_OFF);  /* 关闭 PHY ，进行 PHY 设置*/

    dm9k_phy_write(DM9000_PHY_BMCR, 0x8000);    /* 重置 PHY 的寄存器 */
    while(dm9k_phy_read(DM9000_PHY_BMCR) & 0x8000);
    dm9k_udelay(100);
    dm9k_phy_write(DM9000_PHY_BMCR, phy_reg0);  /*  Tmp */
    dm9k_phy_write(DM9000_PHY_ANAR, phy_reg4);  /* Set PHY media mode */

    dm9k_WriteReg(DM9000_REG_GPCR, GPCR_GEP_CNTL);  /* Let GPIO0 output */
    dm9k_WriteReg(DM9000_REG_GPR, DM9000_PHY_ON);   /* 结束 PHY 设置, 开启 PHY */
}

/*******************************************************************************
*   函数名: dm9k_hash_table
*   参  数: 无
*   返  回: 无
*   功  能: 设置 DM9000A MAC 、 广播 、 多播 寄存器
*/
void dm9k_set_mac(void)
{
    /* 设置 网卡 MAC 位置，来自於 MyHardware */
    dm9k_WriteReg(DM9000_REG_PAR+0, DM9K_MAC_ADDR0);
    dm9k_WriteReg(DM9000_REG_PAR+1, DM9K_MAC_ADDR1);
    dm9k_WriteReg(DM9000_REG_PAR+2, DM9K_MAC_ADDR2);
    dm9k_WriteReg(DM9000_REG_PAR+3, DM9K_MAC_ADDR3);
    dm9k_WriteReg(DM9000_REG_PAR+4, DM9K_MAC_ADDR4);
    dm9k_WriteReg(DM9000_REG_PAR+5, DM9K_MAC_ADDR5);
}

/*******************************************************************************
*   函数名: dm9k_hash_table
*   参  数: 无
*   返  回: 无
*   功  能: 设置 DM9000A MAC 、 广播 、 多播 寄存器
*/
void dm9k_hash_table(void)
{
    uint8_t i;

    for(i = 0; i < 8; i++)                              /* 清除 网卡多播设置 */
    {
        dm9k_WriteReg(DM9000_REG_MAR + i, 0x00);
    }
    dm9k_WriteReg(DM9000_REG_MAR + 7, 0x80);            /* 速设置 广播包 设置 */
}

/*******************************************************************************
*   函数名: dm9k_initFunc
*   参  数: 无
*   返  回: 无
*   功  能: 初始化DM9000AE
*/
void dm9k_initFunc(void)
{
    /* 1:软件复位 */
    dm9k_WriteReg(DM9000_REG_NCR, NCR_RST);         /* 对 DM9000A 进行软件重置 */
    dm9k_udelay(1000);                              /* delay 1ms */
    while((dm9k_ReadReg(DM9000_REG_NCR) & 0x01) != 0);     /* 等待复位完成 */

    /* 2 使能内部PHY */
    /* GPIO0 on pre-activate PHY */
    dm9k_WriteReg(DM9000_REG_GPR, 0x00);                /* power up PHY */
    dm9k_WriteReg(DM9000_REG_GPCR, GPCR_GEP_CNTL);      /* Let GPIO0 output */
    dm9k_WriteReg(DM9000_REG_GPR, 0x00);                /* power up PHY */

    /* 3 NCR 设置 */
    /* Program operating register */
    dm9k_WriteReg(DM9000_REG_NCR, 0x0);     /* only intern phy supported by now */

    /* 4 Clear TX status */
    dm9k_WriteReg(DM9000_REG_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);    /* clear TX status */
    dm9k_WriteReg(DM9000_REG_ISR, 0x0f);    /* Clear interrupt status */

    /* 5 read eeporm data */
    dm9k_phy_mode_set(DM9000_AUTO);                 /* enable SRAM read/write */

    /* 6 set mac */
    dm9k_set_mac();                                 /* 设置 DM9000A MAC */

    dm9k_WriteReg(DM9000_REG_TCR2, 0x80);   /* Switch LED to mode 1 */

    /* 7 set hash table */
    dm9k_hash_table();                              /* 设置 多播*/

    /* 8 set IMR */
    dm9k_WriteReg(DM9000_REG_IMR, IMR_PAR);

    /* 9 set NIC  */
    /* 10 set IMR for interrupt */
    dm9k_WriteReg(DM9000_REG_IMR, IMR_PAR | IMR_PRM);   /* Enable TX/RX interrupt mask */

    /* 11 program RXCR to enable RX */
    dm9k_WriteReg(DM9000_REG_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);   /* RX enable */
}

/*******************************************************************************
*   函数名: dm9k_receive_packet
*   参  数: _uip_buf : 接收缓冲区
*   返  回: > 0 表示接收的数据长度, 0表示没有数据
*   功  能: 读取一包数据
*/
struct pbuf *dm9k_receive_packet(void)
{
    struct pbuf* p = NULL;
    struct pbuf* q = NULL;
    u32 rxbyte = 0;
    vu16 rx_status, rx_length;
    u16* data = NULL;
    u16 dummy = 0;
    int len = 0;

    xSemaphoreTake(s_xLockSemaphore, portMAX_DELAY);  /* 请求互斥量 */

__error_retry:
    dm9k_ReadReg(DM9000_REG_MRCMDX);                    /* 读取内存数据，地址不增加 */
    rxbyte = dm9k_ReadReg(DM9000_REG_MRCMDX);           /* Got most updated data */

    if(rxbyte == DM9000_PKT_RDY)
    {
        /* A packet ready now  & Get status/length */
        NET_REG_ADDR = DM9000_REG_MRCMD;
        rx_status = NET_REG_DATA;
        rx_length = NET_REG_DATA;

        p = pbuf_alloc(PBUF_RAW, rx_length, PBUF_POOL); //pbufs内存池分配pbuf
        if(p != NULL)                                   //内存申请成功
        {
            for(q=p; q!=NULL; q=q->next)
            {
                data = (u16*)q->payload;
                len = q->len;
                while(len > 0)
                {
                    *data = NET_REG_DATA;
                    data++;
                    len -= 2;
                }
            }
        }
        else                                        //内存申请失败
        {
            printk("Dm9k pbuf内存申请失败:%d\r\n",rx_length);
            data = &dummy;
            len = rx_length;
            while(len > 0)
            {
                *data = NET_REG_DATA;
                len -= 2;
            }
        }
        //根据rx_status判断接收数据是否出现如下错误：FIFO溢出、CRC错误
        //对齐错误、物理层错误，如果有任何一个出现的话丢弃该数据帧，
        //当rx_length小于64或者大于最大数据长度的时候也丢弃该数据帧
        if((rx_status & 0XBF00) || (rx_length < 0X40) || (rx_length > DM9K_PKT_MAX))
        {
            if(rx_status & 0x100)
            {
                printk("Dm9k rx fifo error\r\n");
            }
            if(rx_status & 0x200)
            {
                printk("Dm9k rx crc error\r\n");
            }
            if(rx_status & 0x8000)
            {
                printk("Dm9k rx length error\r\n");
            }
            if(rx_length > DM9K_PKT_MAX)
            {
                printk("Dm9k rx length too big\r\n");
                dm9k_WriteReg(DM9000_REG_NCR, NCR_RST); //复位DM9000
                dm9k_udelay(5000);
            }
            if(p != NULL)
            {
                pbuf_free((struct pbuf*)p);    //释放内存
            }
            p = NULL;
            goto __error_retry;
        }
    }
    else  //[ZWX]
    {
        if(rxbyte == DM9000_PKT_NORDY)
        {
            /* clear packet received latch status */
            dm9k_WriteReg(DM9000_REG_ISR, ISR_PRS);

            /* restore receive interrupt */
            dm9k_WriteReg(DM9000_REG_IMR, IMR_PAR | IMR_PRM);
        }
        else
        {
            printk("dm9000 rx: rx error 0x%x, stop device\n", rxbyte);

            dm9k_WriteReg(DM9000_REG_RCR, 0x00);    /* Stop Device */
            dm9k_WriteReg(DM9000_REG_ISR, 0x80);    /* Stop INT request */
            while(1);
        }
    }

    xSemaphoreGive(s_xLockSemaphore);   /* 解锁 */

    return (struct pbuf*)p;
}

/*******************************************************************************
*   函数名: dm9k_send_packet
*   参  数: p_char : 发送数据缓冲区
*           length : 数据长度
*   返  回: 无
*   功  能: 发送一包数据
*/
void dm9k_send_packet(struct pbuf *p)
{
    struct pbuf *q = NULL;
    static uint8_t SendPackOk = 0;
    u16 pbuf_index = 0;
    u8 word[2], word_index = 0;
    int cnt = 0;

    q = p;

    xSemaphoreTake(s_xLockSemaphore, portMAX_DELAY);  /* 请求互斥量 */

    if(SendPackOk == Max_Send_Pack)
    {
        while(dm9k_ReadReg(DM9000_REG_TCR) & TCR_TXREQ)
        {
            dm9k_udelay(5);
        }
        SendPackOk = 0;
    }
    SendPackOk++;

    dm9k_WriteReg(DM9000_REG_IMR, IMR_PAR);             /* 关闭 DM9000A 中断 */

    /*向DM9000的TX SRAM中写入数据，一次写入两个字节数据
      当要发送的数据长度为奇数的时候，我们需要将最后一个字节单独写入DM9000的TX SRAM中 */
    /* 开始将系统的资料搬到到内存中，每次移动一个 word */
    /* Move data to DM9000 TX RAM */
    NET_REG_ADDR = DM9000_REG_MWCMD;
    while(q)
    {
        cnt++;
        if(pbuf_index < q->len)
        {
            word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
            if(word_index == 2)
            {
                NET_REG_DATA = (uint16_t)((word[1] << 8) | word[0]);
                word_index = 0;
            }
        }
        else
        {
            q = q->next;
            pbuf_index = 0;
        }
    }
    if(word_index == 1)
    {
        NET_REG_DATA = (uint16_t)word[0];
    }
    dm9k_WriteReg(DM9000_REG_TXPLL, p->tot_len & 0xff);
    dm9k_WriteReg(DM9000_REG_TXPLH, (p->tot_len >> 8) & 0xff);  /* 设置传送封包的长度 */
    dm9k_WriteReg(DM9000_REG_TCR, TCR_TXREQ);                   /* 进行传送 */
    dm9k_WriteReg(DM9000_REG_IMR, IMR_PAR | IMR_PRM);           /* 开启中断 */

    xSemaphoreGive(s_xLockSemaphore);     /* 解锁 */
}


/**
  * @brief  中断处理函数
  * @param  None
  * @retval None
  */
void Dm9k_IrqHandle(void)
{
    uint8_t  save_reg;
    uint16_t isr_status;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    save_reg = NET_REG_ADDR;                                /* 暂存所使用的位置 */

    isr_status = dm9k_ReadReg(DM9000_REG_ISR);              /* 取得中断产生值 */
    dm9k_WriteReg(DM9000_REG_ISR, isr_status);              /* 清除 DM9000A 中断 */


    /* receive overflow */
    if(isr_status & ISR_ROS)
    {
        printk("dm9k overflow\n");
    }

    if(isr_status & ISR_ROOS)
    {
        printk("dm9k overflow counter overflow\n");
    }

    if(isr_status & ISR_PRS) /* 检查是否为接收中断 */
    {
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xInputSemaphore, &xHigherPriorityTaskWoken);
    }

    NET_REG_ADDR = save_reg;                            /* 恢复所使用的位置 */
}

/*******************************************************************************
*   函数名: dm9k_ReadID
*   参  数: 无
*   返  回: 无
*   功  能: 读取芯片ID
*/
uint32_t dm9k_ReadID(void)
{
    uint8_t vid1,vid2,pid1,pid2;

    vid1 = dm9k_ReadReg(DM9000_REG_VID_L) & 0xFF;
    vid2 = dm9k_ReadReg(DM9000_REG_VID_H) & 0xFF;
    pid1 = dm9k_ReadReg(DM9000_REG_PID_L) & 0xFF;
    pid2 = dm9k_ReadReg(DM9000_REG_PID_H) & 0xFF;

    return (vid2 << 24) | (vid1 << 16) | (pid2 << 8) | pid1;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
u8 bsp_Dm9kGetMode(void)
{
    u8 temp = 0, i = 0;

    while(!(dm9k_phy_read(0X01) & 0X0020))  //等待自动协商完成
    {
        dm9k_udelay(50000);
        i++;
        if(i > 100)
        {
            return 0XFF;                    //自动协商失败
        }
    }
    temp  = ((dm9k_ReadReg(DM9000_REG_NSR)>>6)&0X02);   //获取DM9000的连接速度 00:100M   10:10M
    temp |= ((dm9k_ReadReg(DM9000_REG_NCR)>>3)&0X01);   //获取DM9000的双工状态 1:Full    0 : Half

    return temp;
}


/**
  * @brief
  * @param  None
  * @retval None
  */
int bsp_InitDm9k(void)
{
    uint32_t id = 0;
    uint8_t temp = 0;

    DM9K_GPIOConfig();
    DM9K_FSMCConfig();
    DM9K_NVICConfig();
    id = dm9k_ReadID();
    if(id == DM9000A_ID_OK)
    {
        dm9k_initFunc();

        temp = bsp_Dm9kGetMode();
        if(temp != 0XFF)                            //连接成功，通过串口显示连接速度和双工状态
        {
            printk("DM9000 Speed:%dMbps,Duplex:%s duplex mode\r\n",(temp&0x02)?10:100,(temp&0x01)?"Full":"Half");
        }
        else
        {
            printk("DM9000 Establish Link Failed!\r\n");
        }
    }
    else
    {
        printf("Dm9000 init fail\r\n");
        return DM9000A_InitError;
    }
	return DM9000A_InitOK;
}

