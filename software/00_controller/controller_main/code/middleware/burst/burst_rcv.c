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
	uint32_t			 usb_open_activated;
}burst_rcv_t;


static void burst_rcv_task(void * params);

burst_rcv_t  brcv;

extern void burst_rcv_usb_tx(const char * msg,uint32_t msg_len);

void burst_rcv_init()
{
	memset(&brcv,0,sizeof(brcv));
	brcv.ch_active = -1;
}

void burst_rcv_usb_open()
{
	brcv.usb_open_activated = 1000 / BURST_MUX_TIMEOUT;
}

void burst_rcv_usb_welcome()
{
	if(brcv.usb_open_activated > 0)
	{
		brcv.usb_open_activated--;
		if(brcv.usb_open_activated == 0)
		{
			burst_rcv_usb_tx("Welcome to Unicontroller\r\n",26);
		}
	}
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


void burst_rcv_usb_rx(const char * msg,uint32_t msg_len)
{
	burst_serial_data_t	  * serial_ch;

	serial_ch   = &brcv.ch[CH_USB];

	if(msg_len + serial_ch->RxCnt < sizeof(serial_ch->RxBuffer))
	{
		if(burst_rcv_add_msg(serial_ch,msg,msg_len)!= 0)
		{
			serial_ch->RxMsgCounter++;
			serial_ch->RxMsgOk = 1;
		}

        // TODO - check if really interrupt context
		xSemaphoreGiveFromISR(brcv.sema_recv,NULL);
	}

	brcv.ch_active = CH_USB;
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






void burst_rcv_once()
{
	vSemaphoreCreateBinary(brcv.sema_recv);


	// CH_USB
	// No configuration needed

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


    switch(rcv_ctx->channel)
    {

			case CH_USB:
			{
				burst_rcv_usb_tx(response,length);
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

    execute_store = burst_mux_serial_process(idx,brcv.ch[idx].RxBuffer,brcv.ch[idx].RxCnt);

    if( execute_store!=0)
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
    	burst_rcv_usb_welcome();


        fw_stack_check();
    }
}


