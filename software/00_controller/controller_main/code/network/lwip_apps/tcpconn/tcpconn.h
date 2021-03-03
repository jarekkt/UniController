#ifndef LWIP_TCPCONN_H
#define LWIP_TCPCONN_H

#include "stdint.h"

typedef void (*fn_cb_t)(const char * msg,uint32_t msg_len);


void tcpconn_init(void);
void tcpconn_callback(fn_cb_t  fb_cb);
void tcpconn_send(const char * msg,uint32_t msg_len);




#endif /* LWIP_TCPCONN_H */
