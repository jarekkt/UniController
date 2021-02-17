#include "stm32h7xx_hal.h"
#include "lwip/opt.h"

#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "enc28j60/enc28j60.h"
#include <string.h>


const uint8_t  mac_addr[] =  { MAC_ADDR0,MAC_ADDR1,MAC_ADDR2,MAC_ADDR3,MAC_ADDR4,MAC_ADDR5 };

uint8_t		   send_buffer[1600];
uint8_t		   rcv_buffer[1600];

static void low_level_init(struct netif *netif)
{

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  mac_addr[0];
  netif->hwaddr[1] =  mac_addr[1];
  netif->hwaddr[2] =  mac_addr[2];
  netif->hwaddr[3] =  mac_addr[3];
  netif->hwaddr[4] =  mac_addr[4];
  netif->hwaddr[5] =  mac_addr[5];

  enc28j60Init(mac_addr);

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{

  struct pbuf *q;
  uint32_t framelength = 0;

  /* copy frame from pbufs to driver buffers and send packet */
  for(q = p; q != NULL; q = q->next)
  {
	memcpy(&send_buffer[framelength],q->payload, q->len);
    framelength += q->len;
  }

  if (framelength != p->tot_len)
  {
     return ERR_BUF;
  }
  enc28j60PacketSend(framelength,send_buffer);


  return ERR_OK;
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;
  struct pbuf *q;

  uint32_t framelength;
  uint32_t bufferoffset;

  framelength = enc28j60PacketReceive(sizeof(rcv_buffer),rcv_buffer);

  if (framelength > 0)
  {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, framelength, PBUF_POOL);
  }

  if (p != NULL)
  {
    bufferoffset = 0;

    for(q = p; q != NULL; q = q->next)
    {
      /* Copy data in pbuf */
      memcpy( (uint8_t*)((uint8_t*)q->payload), (uint8_t*)((uint8_t*)rcv_buffer + bufferoffset), q->len);
      bufferoffset = bufferoffset + q->len;
    }
  }

  return p;
}


void ethernetif_input_do(void * arg)
{
	struct netif 	   *netif = (struct netif *)arg;
    TickType_t          xLastWakeTime;
	const  TickType_t   xFrequency = 5 / portTICK_PERIOD_MS;
	uint32_t		    link_check = 0;

    struct pbuf *p;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
       vTaskDelayUntil( &xLastWakeTime, xFrequency );

		do {
			p = low_level_input(netif);
			if (p != NULL)
			{
			  if (netif->input(p, netif) != ERR_OK )
			  {
				pbuf_free(p);
			  }
			}
		}while(p!=NULL);

		if((link_check ++ % 100) == 0)
		{
			if(enc28j60linkup() !=0)
			{
				if((netif->flags & NETIF_FLAG_LINK_UP) == 0)
				{
					netif_set_link_up(netif);
				}
			}
			else
			{
				if((netif->flags & NETIF_FLAG_LINK_UP) != 0)
				{
					netif_set_link_down(netif);
				}
			}
		}
    }
}


err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "uni_controller";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = 'I';
  netif->name[1] = 'F';

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  xTaskCreate( ethernetif_input_do,  "ethInp",  8*configMINIMAL_STACK_SIZE, netif, tskIDLE_PRIORITY   + 2, NULL );

  return ERR_OK;
}


void ethernetif_update_config(struct netif *netif)
{
  if(netif_is_link_up(netif)) {
      /* Restart the EN28J60 module */
      low_level_init(netif);
  }

  ethernetif_notify_conn_changed(netif);
}

__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file
            when the callback is needed,
  */
}


u32_t sys_now(void)
{
	return HAL_GetTick();
}


