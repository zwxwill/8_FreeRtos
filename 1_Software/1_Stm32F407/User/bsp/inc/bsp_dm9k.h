/**
  ******************************************************************************
  * @file    bsp_dm9k.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __BSP_DM9K_H__
#define __BSP_DM9K_H__

#include "stm32f4xx.h"

/* Exported define -----------------------------------------------------------*/
/* DM9000 REGISTER LIST */

/* DM9000 地址 */
#define  NET_BASE_ADDR          0x6C000000
#define  NET_REG_ADDR           (*((volatile uint16_t *) NET_BASE_ADDR))
#define  NET_REG_DATA           (*((volatile uint16_t *) (NET_BASE_ADDR + 0x00200000))) /* 注意这里16字节访问 所以A20对应HA21 */

/* DM9000 REGISTER LIST */
#define DM9000_REG_NCR        0x00
#define DM9000_REG_NSR        0x01
#define DM9000_REG_TCR        0x02
#define DM9000_REG_TSR1       0x03
#define DM9000_REG_TSR2       0x04
#define DM9000_REG_RCR        0x05
#define DM9000_REG_RSR        0x06
#define DM9000_REG_ROCR       0x07
#define DM9000_REG_BPTR       0x08
#define DM9000_REG_FCTR       0x09
#define DM9000_REG_FCR        0x0A
#define DM9000_REG_EPCR       0x0B
#define DM9000_REG_EPAR       0x0C
#define DM9000_REG_EPDRL      0x0D
#define DM9000_REG_EPDRH      0x0E
#define DM9000_REG_WAR        0x0F
#define DM9000_REG_PAR        0x10
#define DM9000_REG_MAR        0x16
#define DM9000_REG_GPCR       0x1E
#define DM9000_REG_GPR        0x1F
#define DM9000_REG_VID_L      0x28
#define DM9000_REG_VID_H      0x29
#define DM9000_REG_PID_L      0x2A
#define DM9000_REG_PID_H      0x2B
#define DM9000_REG_CHIPR      0x2C
#define DM9000_REG_TCR2       0x2D
#define DM9000_REG_OTCR       0x2E
#define DM9000_REG_SMCR       0x2F
#define DM9000_REG_ETXCSR     0x30
#define DM9000_REG_TCSCR      0x31
#define DM9000_REG_RCSCSR     0x32
#define DM9000_REG_MRCMDX     0xF0
#define DM9000_REG_MRCMD      0xF2
#define DM9000_REG_MRRL       0xF4
#define DM9000_REG_MRRH       0xF5
#define DM9000_REG_MWCMDX     0xF6
#define DM9000_REG_MWCMD      0xF8
#define DM9000_REG_MWRL       0xFA
#define DM9000_REG_MWRH       0xFB
#define DM9000_REG_TXPLL      0xFC
#define DM9000_REG_TXPLH      0xFD
#define DM9000_REG_ISR        0xFE
#define DM9000_REG_IMR        0xFF


/* 寄存器配置 */
#define NCR_EXT_PHY         (1<<7)
#define NCR_WAKEEN          (1<<6)
#define NCR_FCOL            (1<<4)
#define NCR_FDX             (1<<3)
#define NCR_LBK             (3<<1)
#define NCR_RST             (1<<0)

#define NSR_SPEED           (1<<7)
#define NSR_LINKST          (1<<6)
#define NSR_WAKEST          (1<<5)
#define NSR_TX2END          (1<<3)
#define NSR_TX1END          (1<<2)
#define NSR_RXOV            (1<<1)

#define TCR_TJDIS           (1<<6)
#define TCR_EXCECM          (1<<5)
#define TCR_PAD_DIS2        (1<<4)
#define TCR_CRC_DIS2        (1<<3)
#define TCR_PAD_DIS1        (1<<2)
#define TCR_CRC_DIS1        (1<<1)
#define TCR_TXREQ           (1<<0)

#define TSR_TJTO            (1<<7)
#define TSR_LC              (1<<6)
#define TSR_NC              (1<<5)
#define TSR_LCOL            (1<<4)
#define TSR_COL             (1<<3)
#define TSR_EC              (1<<2)

#define RCR_WTDIS           (1<<6)
#define RCR_DIS_LONG        (1<<5)
#define RCR_DIS_CRC         (1<<4)
#define RCR_ALL             (1<<3)
#define RCR_RUNT            (1<<2)
#define RCR_PRMSC           (1<<1)
#define RCR_RXEN            (1<<0)

#define RSR_RF              (1<<7)
#define RSR_MF              (1<<6)
#define RSR_LCS             (1<<5)
#define RSR_RWTO            (1<<4)
#define RSR_PLE             (1<<3)
#define RSR_AE              (1<<2)
#define RSR_CE              (1<<1)
#define RSR_FOE             (1<<0)

#define FCTR_HWOT(ot)       (( ot & 0xf ) << 4 )
#define FCTR_LWOT(ot)       ( ot & 0xf )

#define IMR_PAR             (1<<7)
#define IMR_LNKCHGM         (1<<5)
#define IMR_ROOM            (1<<3)
#define IMR_ROM             (1<<2)
#define IMR_PTM             (1<<1)
#define IMR_PRM             (1<<0)

#define ISR_LNKCHGS         (1<<5)
#define ISR_ROOS            (1<<3)
#define ISR_ROS             (1<<2)
#define ISR_PTS             (1<<1)
#define ISR_PRS             (1<<0)
#define ISR_CLR_STATUS      (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)

#define EPCR_REEP           (1<<5)
#define EPCR_WEP            (1<<4)
#define EPCR_EPOS           (1<<3)
#define EPCR_ERPRR          (1<<2)
#define EPCR_ERPRW          (1<<1)
#define EPCR_ERRE           (1<<0)

#define GPCR_GEP_CNTL       (1<<0)

//DM9000内部PHY寄存器
#define DM9000_PHY_BMCR     0X00
#define DM9000_PHY_BMSR     0X01
#define DM9000_PHY_PHYID1   0X02
#define DM9000_PHY_PHYID2   0X03
#define DM9000_PHY_ANAR     0X04
#define DM9000_PHY_ANLPAR   0X05
#define DM9000_PHY_ANER     0X06
#define DM9000_PHY_DSCR     0X10
#define DM9000_PHY_DSCSR    0X11
#define DM9000_PHY_10BTCSR  0X12
#define DM9000_PHY_PWDOR    0X13
#define DM9000_PHY_SCR      0X14

#define DM9000_BYTE_MODE      0x01
#define DM9000_WORD_MODE      0x00
#define DM9000_PHY            0x40
#define DM9000_PKT_RDY        0x01
#define DM9000_PKT_NORDY      0x00
#define DM9000_REG_RESET      0x03
#define DM9000_REG_ONLY_RESET 0x01
#define DM9000_PHY_ON         0x00                /* 设定 PHY 开启 */
#define DM9000_PHY_OFF        0x01                /* 设定 PHY 关闭 */

/* mac 地址 */
#define DM9K_MAC_ADDR0   3
#define DM9K_MAC_ADDR1   0
#define DM9K_MAC_ADDR2   0
#define DM9K_MAC_ADDR3   ( ((*(vu32*)(0x1FFF7A10)) >> 16) & 0XFF)
#define DM9K_MAC_ADDR4   ( ((*(vu32*)(0x1FFF7A10)) >> 8)  & 0XFFF)
#define DM9K_MAC_ADDR5   (  (*(vu32*)(0x1FFF7A10))        & 0XFF)

/* DM9000最大接收包长度 */
#define DM9K_PKT_MAX             1536

/* DM9000A 传送函数设置宏 */
#define Max_Send_Pack           2

/* DM9000A ID */
#define DM9000A_ID_OK         0x0A469000

enum DM9000_PHY_mode
{
    DM9000_10MHD = 0,
    DM9000_100MHD = 1,
    DM9000_10MFD = 4,
    DM9000_100MFD = 5,
    DM9000_AUTO  = 8,
    DM9000_1M_HPNA = 0x10
};

/* 相关宏设置 */
typedef enum
{
    DM9000A_InitOK  = 0,
    DM9000A_InitError
} E_DM9000InitType;



/* Exported typedef ----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int bsp_InitDm9k(void);
struct pbuf *dm9k_receive_packet(void);
void dm9k_send_packet(struct pbuf *p);
void Dm9k_IrqHandle(void);

#endif

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

