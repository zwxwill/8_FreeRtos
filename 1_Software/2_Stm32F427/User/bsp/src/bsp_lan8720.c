/**
  ******************************************************************************
  * @file    bsp_lan8720.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "bsp_lan8720.h"
#include "bsp.h"


#define LAN8720_PHY_ADDRESS        0x00             //LAN8720 PHYÐ¾Æ¬µØÖ·.
#define LAN8720_RST_ENABLE        (GPIOA->BSRRH = GPIO_Pin_0 )
#define LAN8720_RST_DISABLE       (GPIOA->BSRRL = GPIO_Pin_0 )


static void Delay_ns(u32 n);
static void Ethernet_NVICConfiguration(void);
static u8 ETH_MACDMA_Config(void);


/**
  * @brief
  * @param
  * @retval
  */
static void Delay_ns(u32 n)
{
    u32 i = 0, j = 0;

    for(i=0; i<50; i++)
        for(j=0; j<n; j++);
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void bsp_InitLan8720(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);

    /*ÍøÂçÒý½ÅÉèÖÃ RMII½Ó¿Ú
      ETH_MDIO -------------------------> PA2
      ETH_MDC --------------------------> PC1
      ETH_RMII_REF_CLK------------------> PA1
      ETH_RMII_CRS_DV ------------------> PA7
      ETH_RMII_RXD0 --------------------> PC4
      ETH_RMII_RXD1 --------------------> PC5
      ETH_RMII_TX_EN -------------------> PB11
      ETH_RMII_TXD0 --------------------> PB12
      ETH_RMII_TXD1 --------------------> PB13
      ETH_RESET-------------------------> PA0*/

    //ÅäÖÃPA1 PA2 PA7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

    //ÅäÖÃPC1,PC4 and PC5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

    //ÅäÖÃPB11, PB12 and PB13
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);

    //ÅäÖÃPD3ÎªÍÆÍêÊä³ö
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    LAN8720_RST_ENABLE;
    Delay_ns(50000);
    LAN8720_RST_DISABLE;
    Ethernet_NVICConfiguration();
    ETH_MACDMA_Config();
}

/**
  * @brief
  * @param
  * @retval
  */
static void Ethernet_NVICConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ETH_IRQ_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief
  * @param
  * @retval
  */
static u8 ETH_MACDMA_Config(void)
{
    u8 rval;
    ETH_InitTypeDef ETH_InitStructure;

    /* Enable Ethernet clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    ETH_DeInit();                               /* AHB Bus reset to start ethernet  */
    ETH_SoftwareReset();                        /* software restart ethernet  */
    while(ETH_GetSoftwareResetStatus() == SET); /* wait for the software reset finish */
    ETH_StructInit(&ETH_InitStructure);

    /* Ethernet MAC configuration */
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;        /* close retry */
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable; /* Enable broadcast */
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

    /* receive store and transmit store configuration */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    rval=ETH_Init(&ETH_InitStructure, LAN8720_PHY_ADDRESS);

    if(rval == ETH_SUCCESS)
    {
        ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);
    }
    return rval;
}

/**
  * @brief
  * @param
  * @retval
  */
u8 Lan8720_GetSpeed(void)
{
    u8 speed = 0;

    speed = ((ETH_ReadPHYRegister(0x00, 31) & 0x1C) >> 2);
    return speed;
}





