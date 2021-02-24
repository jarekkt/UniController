/*!
    \file burst_rcv.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"



#define BURST_MUX_TIMEOUT 10000




typedef struct
{
    char    	RxBuffer[512];
    int      	RxCnt;
    int      	RxMsgCounter;
    int      	RxMsgOk;


    uint32_t    serial_id;

}burst_serial_data_t;

typedef struct
{
	burst_serial_data_t  ch[CH_CNT];
	xSemaphoreHandle    sema_recv;

}burst_rcv_t;


static void burst_rcv_task(void * params);

burst_rcv_t  brcv;

extern void burst_rcv_usb_tx(char * msg,uint32_t msg_len);

void burst_rcv_init()
{
	memset(&brcv,0,sizeof(brcv));
}


static void burst_rcv_cc(uint8_t cc,ch_idx_e idx,portBASE_TYPE * woken)
{
   burst_serial_data_t	  * serial_ch;

   serial_ch   = &brcv.ch[idx];

   if(serial_ch->RxMsgOk == 0)
   {
      if(serial_ch->RxCnt < sizeof(serial_ch->RxBuffer))
      {
          serial_ch->RxBuffer[serial_ch->RxCnt++] = cc;

          if( (cc == '>') || (cc == '\n') || (cc == '\r'))
          {
                  serial_ch->RxMsgCounter++;
                  serial_ch->RxMsgOk = 1;

                  xSemaphoreGiveFromISR(brcv.sema_recv,woken);
          }
      }
      else
      {
          serial_ch->RxCnt = 0;
      }
   }
}

void burst_rcv_usb_rx(char * msg,uint32_t msg_len)
{
	burst_serial_data_t	  * serial_ch;

	serial_ch   = &brcv.ch[CH_USB];

	if(msg_len < sizeof(serial_ch->RxBuffer))
	{
		memcpy(serial_ch->RxBuffer,msg,msg_len);
		serial_ch->RxCnt = msg_len;
        serial_ch->RxMsgCounter++;
        serial_ch->RxMsgOk = 1;

        // TODO - check if really interrupt context
		xSemaphoreGiveFromISR(brcv.sema_recv,NULL);
	}
}

void burst_rcv_eth_rx(char * msg,uint32_t msg_len)
{
	burst_serial_data_t	  * serial_ch;

	serial_ch   = &brcv.ch[CH_ETH];

	if(msg_len < sizeof(serial_ch->RxBuffer))
	{
		memcpy(serial_ch->RxBuffer,msg,msg_len);
		serial_ch->RxCnt = msg_len;
        serial_ch->RxMsgCounter++;
        serial_ch->RxMsgOk = 1;

        // TODO - check if really interrupt context
		xSemaphoreGiveFromISR(brcv.sema_recv,NULL);
	}
}




static void burst_rcv_cc_debug(uint32_t portId,uint8_t cc,portBASE_TYPE * woken)
{
	burst_rcv_cc(cc,CH_DEBUG,woken);
}

static void burst_rcv_cc_rs485(uint32_t portId,uint8_t cc,portBASE_TYPE * woken)
{
	burst_rcv_cc(cc,CH_RS485,woken);
}



void burst_rcv_once()
{
	vSemaphoreCreateBinary(brcv.sema_recv);

	// CH_DEBUG
	brcv.ch[CH_DEBUG].serial_id	= SRV_SERIAL_DEBUG;
	srv_serial_rcv_callback(SRV_SERIAL_DEBUG,burst_rcv_cc_debug);

	// CH_RS485
	brcv.ch[CH_RS485].serial_id	= SRV_SERIAL_RS485;
	srv_serial_485_rcv_callback(SRV_SERIAL_RS485,burst_rcv_cc_rs485);
	srv_serial_485_enable(SRV_SERIAL_RS485,1);


	// CH_USB
	// No configuration needed
	brcv.ch[CH_USB].serial_id 		= -1;

	xTaskCreate( burst_rcv_task, "Burst", 6 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 1, NULL );
}


/*!
        \brief Function which calculates response frame crc and sends it out.

*/
void burst_rcv_send_response(const burst_rcv_ctx_t * rcv_ctx,char * response, int length)
{
	if(length < 0)
	{
		length = strlen(response);
	}


    switch(rcv_ctx->channel)
    {
			case CH_RS485:
			{
				srv_serial_485_send(brcv.ch[rcv_ctx->channel].serial_id,response,length);
			}break;

			case CH_USB:
			{
				burst_rcv_usb_tx(response,length);
			}break;

			case CH_DEBUG:
			{
				srv_serial_send(brcv.ch[rcv_ctx->channel].serial_id,response,length);
			}break;

			default:
			{
			}break;
    }

}


static void burst_rcv_serial_process(ch_idx_e idx)
{
	uint32_t execute_store;

    execute_store = burst_mux_serial_process(idx,brcv.ch[idx].RxBuffer,brcv.ch[idx].RxCnt);

    if( (idx == CH_DEBUG) && (execute_store!=0) )
	{
		tsk_storage_activate();
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
			}
			brcv.ch[idx].RxCnt 	= 0;
			brcv.ch[idx].RxMsgOk = 0;
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


