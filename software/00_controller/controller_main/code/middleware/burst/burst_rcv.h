/*!
    \file burst_rcv.h


*/


#ifndef BURST_RCV_H
#define BURST_RCV_H


#include "system.h"

typedef enum
{
	CH_FTDI,
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
	uint32_t 		channel:8;
	uint32_t		frame_format:8;
}burst_rcv_ctx_t;

void burst_rcv_init();
void burst_rcv_once();
void burst_rcv_can_message();
void burst_rcv_send_response(const burst_rcv_ctx_t * rcv_ctx,const char * response, int length);

void burst_rcv_usb_rx(const char * msg,uint32_t msg_len);
void burst_rcv_usb_tx(const char * msg,uint32_t msg_len);
void burst_rcv_usb_open();


#endif //BURST_RCV_H
