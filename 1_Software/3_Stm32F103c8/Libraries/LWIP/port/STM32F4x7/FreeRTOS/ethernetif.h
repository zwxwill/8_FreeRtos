#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
void dm9k_ethernetif_input(void * pvParameters);
err_t dm9k_ethernetif_init(struct netif *netif);

/* MAC ADDRESS*/
#define LAN8720_MAC_ADDR0   2
#define LAN8720_MAC_ADDR1   0
#define LAN8720_MAC_ADDR2   0
#define LAN8720_MAC_ADDR3   ( ((*(vu32*)(0x1FFF7A10)) >> 16) & 0XFF)
#define LAN8720_MAC_ADDR4   ( ((*(vu32*)(0x1FFF7A10)) >> 8)  & 0XFFF)
#define LAN8720_MAC_ADDR5   (  (*(vu32*)(0x1FFF7A10))        & 0XFF)


#define DM9K_MAC_ADDR0       3
#define DM9K_MAC_ADDR1       0
#define DM9K_MAC_ADDR2       0
#define DM9K_MAC_ADDR3       ( ((*(vu32*)(0x1FFF7A10)) >> 16) & 0XFF)
#define DM9K_MAC_ADDR4       ( ((*(vu32*)(0x1FFF7A10)) >> 8)  & 0XFFF)
#define DM9K_MAC_ADDR5       (  (*(vu32*)(0x1FFF7A10))        & 0XFF)

#endif
