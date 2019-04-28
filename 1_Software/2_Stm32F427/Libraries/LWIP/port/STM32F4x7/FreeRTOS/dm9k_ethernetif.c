/**
* @file
* Ethernet Interface Skeleton
*
*/

/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*
*/

/*
* This file is a skeleton for developing Ethernet network interface
* drivers for lwIP. Add code to the low_level functions and do a
* search-and-replace for the word "ethernetif" to replace it with
* something that better describes your network interface.
*/
#include "ethernetif.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/timers.h"
#include "netif/etharp.h"
#include "err.h"
#include "srv_netconf.h"

#include "bsp_dm9k.h"

#include "ethernetif.h"
#include "stm32f4x7_eth.h"
#include <string.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IFNAME2                                 'd'
#define IFNAME3                                 'm'

#define netifMTU                                ( 1500 )
#define netifDm9k_TASK_STACK_SIZE               ( 1024 )
#define netifDm9k_TASK_PRIORITY                 ( configMAX_PRIORITIES - 3 )
#define netifDm9kGUARD_BLOCK_TIME               ( 250 )
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct netif *s_pxDm9kNetIf = NULL;    /* netif 指针 */
xSemaphoreHandle s_xInputSemaphore = NULL;    /* netif 输入信号量 */
xSemaphoreHandle s_xLockSemaphore = NULL;     /* mutex互斥信号量 */
static TaskHandle_t xHandleTaskNetifInput = NULL; /* neitif 输入任务 */
/* Private functions ---------------------------------------------------------*/
static void arp_timer(void *arg);



/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf * dm9k_low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;

    p = dm9k_receive_packet();

    return p;
}

/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/

static err_t dm9k_low_level_output(struct netif *netif, struct pbuf *p)
{
    dm9k_send_packet(p);

    return ERR_OK;
}

/**
* This function is the ethernetif_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function low_level_input()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/
void dm9k_ethernetif_input(void * pvParameters)
{
    struct pbuf *p;

    while(1)
    {
        if(xSemaphoreTake(s_xInputSemaphore, (portTickType)portMAX_DELAY) == pdTRUE)    //[ZWX]
        {
            while(1)
            {
                p = dm9k_low_level_input(s_pxDm9kNetIf);
                if(p != NULL)
                {
                    if(ERR_OK != s_pxDm9kNetIf->input(p, s_pxDm9kNetIf))
                    {
                        printf("dm9k_ethernetif_input: IP input error\n");
                        pbuf_free(p);
                        p = NULL;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
}

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void dm9k_low_level_init(struct netif *netif)
{
    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    netif->hwaddr[0] =  DM9K_MAC_ADDR0;
    netif->hwaddr[1] =  DM9K_MAC_ADDR1;
    netif->hwaddr[2] =  DM9K_MAC_ADDR2;
    netif->hwaddr[3] =  DM9K_MAC_ADDR3;
    netif->hwaddr[4] =  DM9K_MAC_ADDR4;
    netif->hwaddr[5] =  DM9K_MAC_ADDR5;

    /* set netif maximum transfer unit */
    netif->mtu = netifMTU;

    /* Accept broadcast address and ARP traffic */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    s_pxDm9kNetIf = netif;

    /* create binary semaphore used for informing ethernetif of frame reception */
    if(s_xInputSemaphore == NULL)
    {
        s_xInputSemaphore = xSemaphoreCreateCounting(10, 0);
    }

    s_xLockSemaphore = xSemaphoreCreateMutex();  /* 创建互斥信号量 */
    if(s_xLockSemaphore == NULL)
    {
        printf("s_xDm9kLockSemaphore fail!\r\n");
    }

    /* create the task that handles the ETH_MAC */
    xTaskCreate(dm9k_ethernetif_input,
                "Dm9k_Eth_Input",
                netifDm9k_TASK_STACK_SIZE,
                NULL,
                netifDm9k_TASK_PRIORITY,
                &xHandleTaskNetifInput);

    if(DM9000A_InitError == bsp_InitDm9k())     /* dm9000初始化 */
    {
        printf("DM9000 init failed!\r\n");
    }
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t dm9k_ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME2;
    netif->name[1] = IFNAME3;

    netif->output = etharp_output;
    netif->linkoutput = dm9k_low_level_output;
    /* initialize the hardware */
    dm9k_low_level_init(netif);

    etharp_init();
    sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);

    return ERR_OK;
}

static void arp_timer(void *arg)
{
    etharp_tmr();
    sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}



