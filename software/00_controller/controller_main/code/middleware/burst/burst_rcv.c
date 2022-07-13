/*!
    \file burst_rcv.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "tcpconn/tcpconn.h"


#define BURST_MUX_TIMEOUT 100


typedef struct
{
    char    	RxBuffer[2048];
    int      	RxCnt;
    int      	RxMsgCounter;
    int      	RxMsgOk;


}burst_serial_data_t;

typedef struct
{
	burst_serial_data_t  ch[CH_CNT];
	int32_t			 	 ch_active;
	xSemaphoreHandle     sema_recv;
}burst_rcv_t;


static void burst_rcv_task(void * params);

burst_rcv_t  brcv;


void burst_rcv_init()
{
	memset(&brcv,0,sizeof(brcv));
	brcv.ch_active = -1;
}



uint32_t	burst_rcv_add_msg(burst_serial_data_t * ch,const char * msg,uint32_t msg_len)
{
	uint32_t	eom = 0;
	uint32_t	idx;

	for(idx = ch->RxCnt ;idx < ch->RxCnt + msg_len;idx++ )
	{
		ch->RxBuffer[idx] = *msg;
		if( (*msg == '\n') || (*msg == '\r'))
		{
			eom = 1;
		}
		msg++;
	}
	ch->RxCnt += msg_len;

	return eom;
}



void burst_rcv_eth_rx(const char * msg,uint32_t msg_len)
{
	burst_serial_data_t	  * serial_ch;

	serial_ch   = &brcv.ch[CH_ETH];

	if(msg_len < sizeof(serial_ch->RxBuffer))
	{
		if(burst_rcv_add_msg(serial_ch,msg,msg_len)!= 0)
		{
			serial_ch->RxMsgCounter++;
			serial_ch->RxMsgOk = 1;

			xSemaphoreGiveFromISR(brcv.sema_recv,NULL);
		}
	}

	brcv.ch_active = CH_ETH;
}


static void burst_rcv_cc_debug(uint32_t port_id,uint8_t cc,portBASE_TYPE * pb)
{
   burst_serial_data_t	  * serial_ch;

   serial_ch   = &brcv.ch[CH_FTDI];

   if(serial_ch->RxMsgOk == 0)
   {
      if(serial_ch->RxCnt < sizeof(serial_ch->RxBuffer))
      {
          serial_ch->RxBuffer[serial_ch->RxCnt++] = cc;

          if( (cc == '>') || (cc == '\n') || (cc == '\r'))
          {
                  serial_ch->RxMsgCounter++;
                  serial_ch->RxMsgOk = 1;

                  xSemaphoreGiveFromISR(brcv.sema_recv,NULL);
          }
      }
      else
      {
          serial_ch->RxCnt = 0;
      }
   }
}




void burst_rcv_once()
{
	vSemaphoreCreateBinary(brcv.sema_recv);


	// CH_FTDI
	srv_serial_rcv_callback(SRV_SERIAL_DEBUG,burst_rcv_cc_debug);

	// CH_ETH
	tcpconn_callback(burst_rcv_eth_rx);


	xTaskCreate( burst_rcv_task, "Burst", 6 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 1, NULL );
}


/*!
        \brief Function which calculates response frame crc and sends it out.

*/
void burst_rcv_send_response(const burst_rcv_ctx_t * rcv_ctx,const char * response, int length)
{
	if(length < 0)
	{
		length = strlen(response);
	}


	printd(LVL_DEBUG,"Comm sent[%d]: %s",length,response);


    switch(rcv_ctx->channel)
    {

			case CH_FTDI:
			{
				srv_serial_send(SRV_SERIAL_DEBUG,response,length);
			}break;

			case CH_ETH:
			{
				tcpconn_send(response,length);
			}break;


			default:
			{
			}break;
    }

}


static void burst_rcv_serial_process(ch_idx_e idx)
{
	uint32_t execute_store;

	brcv.ch[idx].RxBuffer[brcv.ch[idx].RxCnt] = 0;

	printd(LVL_DEBUG,"Comm received[%d]: %s",brcv.ch[idx].RxCnt,brcv.ch[idx].RxBuffer);


    execute_store = burst_mux_serial_process(idx,brcv.ch[idx].RxBuffer,brcv.ch[idx].RxCnt);

    if( execute_store!=0)
	{
    	// Store data to flash
		tsk_storage_activate();

		// Reinitialize blocks with possible dependencies
		srv_gpio_refresh();
	}
}



static void burst_rcv_serial_rcv(void)
{
    uint32_t cnt= 0;
    ch_idx_e idx;


    xSemaphoreTake(brcv.sema_recv,BURST_MUX_TIMEOUT/portTICK_RATE_MS);

    do
    {
		cnt = 0;
		for(idx=0;idx < CH_CNT;idx++)
		{
			if(brcv.ch[idx].RxMsgOk != 0)
			{
				burst_rcv_serial_process(idx);
				cnt++;

				brcv.ch[idx].RxCnt 	= 0;
				brcv.ch[idx].RxMsgOk = 0;
			}
		}


    }while (cnt != 0);

}


/*!
        \brief Burst task function. Sends periodically burst frames ( if enabled by PC message).

*/
static void burst_rcv_task(void * params)
{
    while(1)
    {

    	burst_rcv_serial_rcv();
        fw_stack_check();
    }
}


