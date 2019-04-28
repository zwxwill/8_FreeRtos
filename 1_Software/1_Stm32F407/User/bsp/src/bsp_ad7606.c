/**
  ******************************************************************************
  * @file    bsp_ad7606.c
  * @author  zwx
  * @version V1.1.0
  * @date    31-July-2013
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_ad7606.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief
  * @param  None
  * @retval None
  */
void Bsp_InitAd7606(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);       //使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);        //使能SPI1时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;                  //PB7-BUSY
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                //普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;           //低速模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;            //无上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                      //初始化PB7

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9; //PB6/CS, PB8/RESET, PB9/CONVEST
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                //输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;            //低速模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                 //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                       //初始化PB6,PB8,PB9

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;    //PB3, PB4复用功能输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  //初始化

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3复用为 SPI1-SCK
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4复用为 SPI1-MISO

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                   //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;      //设置SPI的数据大小:SPI发送接收16位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                     //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                    //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;    //定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;      //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                            //CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);                                     //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE);                                                              //使能SPI外设
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ad7606Reset(void)
{
    AD7606_RESET=0; /* 退出复位状态 */

    AD7606_RESET=1; /* 进入复位状态 */
    AD7606_RESET=1; /* 延迟。 RESET复位高电平脉冲宽度最小50ns。 */
    AD7606_RESET=1;
    AD7606_RESET=1;

    AD7606_RESET=0; /* 退出复位状态 */
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ad7606StartConv(void)
{
    /* datasheet page 7：  CONVST 高电平脉冲宽度和低电平脉冲宽度最短 25ns */
    AD7606_CONVST=0;
    AD7606_CONVST=0;
    AD7606_CONVST=0;

    /* CONVST平时为高 */
    AD7606_CONVST=1;
}


/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_Ad7606ReadCurAdc(uint16_t* ADC_buff)
{
    uint8_t  Channelx=0;
    uint16_t ADC_Value=0;

    /* 等待AD转换完成，BUSY变低 */
    while(AD7606_BUSY == 1);
    /* SPI1片选使能 */
    AD7606_CS = 0;
    /* 采集8个通道，每个通道16个时钟周期 */
    for(Channelx=0; Channelx<8; Channelx++)
    {
        /* 等待发送区空 */
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
        /* 通过外设SPIx发送一个word 数据 */
        SPI_I2S_SendData(SPI1, 0xffff);
        /* 等待接收完一个word */
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}
        ADC_Value=SPI_I2S_ReceiveData(SPI1);
        /* 负电压噪声，强制为0 */
        if((ADC_Value&0x8000)==0x8000)
        {
            ADC_Value=0;
        }
        ADC_buff[Channelx]=ADC_Value;
    }
    /* SPI1片选禁能 */
    AD7606_CS = 1;
    /* 启动转换 */
    bsp_Ad7606StartConv();
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
