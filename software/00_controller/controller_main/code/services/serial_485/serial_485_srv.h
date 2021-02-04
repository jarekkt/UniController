#ifndef SERIAL__SRV_H
#define SERIAL_485_SRV_H

/*! \file   serial_485_srv.h

    \brief  Header for 485 serial port driver
    
    
*/

#include "system.h"

#define SRV_SERIAL_485_UxART1_ID       0
#define SRV_SERIAL_485_UxART2_ID       1


#define SRV_SERIAL_RS485_1		 	  	SRV_SERIAL_485_UxART1_ID
#define SRV_SERIAL_RS485_2		   		SRV_SERIAL_485_UxART2_ID

void    srv_serial_485_init(void);
void    srv_serial_485_once(void);
void    srv_serial_485_enable(uint32_t port_id,int enable);

typedef  void ( * serial_rec_485_char)(uint32_t port_id,uint8_t,portBASE_TYPE *);

void    srv_serial_485_rcv_callback(uint32_t port_id,serial_rec_485_char );


void    srv_serial_485_send(uint32_t port_id,const char * buffer,int length);
void    srv_serial_485_send_blocked(uint32_t port_id,const char * buffer,int length);
void    srv_serial_485_puts(uint32_t port_id,const char * buffer,int length);




#endif // SERIAL_SRV_H
