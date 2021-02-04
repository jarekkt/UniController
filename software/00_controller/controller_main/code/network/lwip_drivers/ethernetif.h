#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"
#include "main.h"

#define  MAC_ADDR0	0x40
#define  MAC_ADDR1  0x74
#define  MAC_ADDR2  0xE0
#define  MAC_ADDR3  0x26
#define  MAC_ADDR4  0x89
#define  MAC_ADDR5  0xa0



/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);
void  ethernetif_update_config(struct netif *netif);
void  ethernetif_notify_conn_changed(struct netif *netif);






#endif
