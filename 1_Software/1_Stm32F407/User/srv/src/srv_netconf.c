/**
  ******************************************************************************
  * @file    netconf.c
  * @author  zwx
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Network connection configuration
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "main.h"
#include "srv_netconf.h"
#include "tcpip.h"
#include <stdio.h>
#include "lwip/sockets.h"
#include "heap_6.h"
#include "stm32f4x7_eth.h"

#ifndef LWIP_USE_INTERN_RAM
	/* Ethernet Rx & Tx DMA Descriptors */
	/* ethernet DMA receive descriptor */
	extern ETH_DMADESCTypeDef *DMARxDscrTab;
	/* ethernet DMA send descriptor */
	extern ETH_DMADESCTypeDef *DMATxDscrTab;
	/* ethernet DMA receive buffer */
	extern uint8_t *Rx_Buff;
	/* ethernet DMA send buffer */
	extern uint8_t *Tx_Buff;
	
	extern u8_t *memp_memory;
	extern u8_t *ram_heap;
	extern u32_t memp_get_memorysize(void);
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct netif xLan8720Netif; /* network interface structure */
static struct netif xDm9kNetif; /* network interface structure */
static T_NetConfig g_tNetCfg[2] = 
{
	{
		{192, 168, 1, 44},   /* IP */
		{255, 255, 255, 0},  /* mask */
		{192, 168, 1, 1},    /* gateway */    
	},
	{
		{192, 168, 1, 45},   /* IP */
		{255, 255, 255, 0},  /* mask */
		{192, 168, 1, 1},    /* gateway */    		
	}
};
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_SetNetNonBlock(int sock)
{
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void Srv_SetNetBlock(int sock)
{
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
	flags &= (~O_NONBLOCK);
    fcntl(sock, F_SETFL, flags);	
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static PT_NetConfig Srv_GetDefaultNetCfg(void)
{
	return g_tNetCfg;
}

void ETH_Mem_Free(void)
{
    vMyPortFree(DMARxDscrTab, SRAM_EX);
    vMyPortFree(DMATxDscrTab, SRAM_EX);
    vMyPortFree(Rx_Buff, SRAM_EX);
    vMyPortFree(Tx_Buff, SRAM_EX);
}

u8 ETH_Mem_Malloc(void)
{
    DMARxDscrTab = pvMyPortMalloc(ETH_RXBUFNB*sizeof(ETH_DMADESCTypeDef), SRAM_EX);
    DMATxDscrTab = pvMyPortMalloc(ETH_TXBUFNB*sizeof(ETH_DMADESCTypeDef), SRAM_EX);
    Rx_Buff = pvMyPortMalloc(ETH_RX_BUF_SIZE*ETH_RXBUFNB, SRAM_EX);
    Tx_Buff = pvMyPortMalloc(ETH_TX_BUF_SIZE*ETH_TXBUFNB, SRAM_EX);

    if(!DMARxDscrTab||!DMATxDscrTab||!Rx_Buff||!Tx_Buff)
    {
        ETH_Mem_Free();
        return 1;   //fail
    }

    return 0;   //  success
}

void LWIP_Mem_Free(void)
{
    vMyPortFree(memp_memory, SRAM_EX);
    vMyPortFree(ram_heap, SRAM_EX);
}


u8 LWIP_Mem_Malloc(void)
{
    u32 mempsize = 0;
    u32 ramheapsize = 0;
    mempsize = memp_get_memorysize();         //得到memp_memory数组大小
    memp_memory = pvMyPortMalloc(mempsize, SRAM_EX);    //为memp_memory申请内存
    ramheapsize = LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到ram heap大小
    ram_heap = pvMyPortMalloc(ramheapsize, SRAM_EX);   //为ram_heap申请内存
    if(!memp_memory||!ram_heap)//有申请失败的
    {
        LWIP_Mem_Free();
        return 1;
    }
    return 0;
}


/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void Srv_LwIPInit(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    T_NetConfig *tNetCfg;

#ifndef LWIP_USE_INTERN_RAM
	ETH_Mem_Malloc();
    LWIP_Mem_Malloc();
#endif
	
    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

    /* 获取通信配置信息 */
    tNetCfg = Srv_GetDefaultNetCfg();

    /* IP address setting */
	IP4_ADDR(&ipaddr, tNetCfg[1].ip[0], tNetCfg[1].ip[1], tNetCfg[1].ip[2], tNetCfg[1].ip[3]);
	IP4_ADDR(&netmask, tNetCfg[1].mask[0], tNetCfg[1].mask[1] , tNetCfg[1].mask[2], tNetCfg[1].mask[3]);
	IP4_ADDR(&gw, tNetCfg[1].gateway[0], tNetCfg[1].gateway[1], tNetCfg[1].gateway[2], tNetCfg[1].gateway[3]);

	printf("Lan8720 IP.............%d.%d.%d.%d\r\n",tNetCfg[1].ip[0],tNetCfg[1].ip[1],tNetCfg[1].ip[2],tNetCfg[1].ip[3]);
	printf("Lan8720 NetMask........%d.%d.%d.%d\r\n",tNetCfg[1].mask[0],tNetCfg[1].mask[1],tNetCfg[1].mask[2],tNetCfg[1].mask[3]);
	printf("Lan8720 GateWay........%d.%d.%d.%d\r\n",tNetCfg[1].gateway[0],tNetCfg[1].gateway[1],tNetCfg[1].gateway[2],tNetCfg[1].gateway[3]);

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	struct ip_addr *netmask, struct ip_addr *gw,
	void *state, err_t (* init)(struct netif *netif),
	err_t (* input)(struct pbuf *p, struct netif *netif))

	Adds your network interface to the netif_list. Allocate a struct
	netif and pass a pointer to this structure as the first argument.
	Give pointers to cleared ip_addr structures when using DHCP,
	or fill them with sane numbers otherwise. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/
	netif_add(&xLan8720Netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface.*/
	netif_set_default(&xLan8720Netif);
	netif_set_up(&xLan8720Netif);

#ifdef DM9000_EN
    /* Dm9000 网卡配置 */
    /* IP address setting */
    IP4_ADDR(&ipaddr, tNetCfg[0].ip[0], tNetCfg[0].ip[1], tNetCfg[0].ip[2], tNetCfg[0].ip[3]);
    IP4_ADDR(&netmask, tNetCfg[0].mask[0], tNetCfg[0].mask[1], tNetCfg[0].mask[2], tNetCfg[0].mask[3]);
    IP4_ADDR(&gw, tNetCfg[0].gateway[0], tNetCfg[0].gateway[1], tNetCfg[0].gateway[2], tNetCfg[0].gateway[3]);

    printf("DM9000 IP..............%d.%d.%d.%d\r\n",tNetCfg[0].ip[0],tNetCfg[0].ip[1],tNetCfg[0].ip[2],tNetCfg[0].ip[3]);
    printf("DM9000 NETMASK.........%d.%d.%d.%d\r\n",tNetCfg[0].mask[0],tNetCfg[0].mask[1],tNetCfg[0].mask[2],tNetCfg[0].mask[3]);
    printf("DM9000 GATEWAY.........%d.%d.%d.%d\r\n",tNetCfg[0].gateway[0],tNetCfg[0].gateway[1],tNetCfg[0].gateway[2],tNetCfg[0].gateway[3]);

    netif_add(&xDm9kNetif, &ipaddr, &netmask, &gw, NULL, &dm9k_ethernetif_init, &tcpip_input);

    /*  Registers the default network interface.*/
    netif_set_default(&xDm9kNetif);
    netif_set_up(&xDm9kNetif);
#endif
}

/**
  * @brief
  * @param  None
  * @retval None
  */
struct netif *Srv_GetLan8720Netif(void)
{
    return &xLan8720Netif;
}


/**
  * @brief
  * @param  None
  * @retval None
  */
struct netif *Srv_GetDm9kNetif(void)
{
    return &xDm9kNetif;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
