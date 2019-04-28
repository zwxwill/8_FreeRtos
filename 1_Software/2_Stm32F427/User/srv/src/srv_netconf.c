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


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct netif xLan8720Netif; /* network interface structure */
static struct netif xDm9kNetif; /* network interface structure */
static T_NetConfig g_tNetCfg[2] = 
{
	{
		{192, 168, 1, 31},   /* IP */
		{255, 255, 255, 0},  /* mask */
		{192, 168, 1, 1},    /* gateway */    
	},
	{
		{192, 168, 1, 30},   /* IP */
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
void Srv_SetNetBlock(int sock)
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
void Srv_SetNetNonBlock(int sock)
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
