/*!
    \file burst_rcv.h


*/


#ifndef BURST_RCV_H
#define BURST_RCV_H


#include "system.h"

typedef enum
{
	CH_DEBUG,
	CH_RS485,
	CH_USB,
	CH_ETH,
	CH_CNT
}ch_idx_e;

typedef enum
{
	RCV_FRAME_DIRECT = 0,
	RCV_FRAME_ENCAPSULATED
}rcv_frame_e;


typedef struct
{
	ch_idx_e 		channel;
	rcv_frame_e		frame_format;
	uint32_t		address;
}burst_rcv_ctx_t;

void burst_rcv_init();
void burst_rcv_once();
void burst_rcv_send_response(const burst_rcv_ctx_t * rcv_ctx,char * response, int length);

void burst_rcv_usb_rx(char * msg,uint32_t msg_len);
void burst_rcv_usb_tx(char * msg,uint32_t msg_len);



#endif //BURST_RCV_H
