/**
  ******************************************************************************
  * @file    bsp_led.h
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#ifndef __SRV_NETCONF_H__
#define __SRV_NETCONF_H__

#include "stm32f4xx.h"

typedef struct NetConfig
{
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gateway[4];	
}T_NetConfig, *PT_NetConfig;


/* Exported functions ---------------------------------------------------------*/
void Srv_LwIPInit(void);
struct netif *Srv_GetLan8720Netif(void);
struct netif *Srv_GetDm9kNetif(void);
void Srv_SetNetBlock(int sock);
void Srv_SetNetNonBlock(int sock);

#endif

/************************************** end *************************************/
