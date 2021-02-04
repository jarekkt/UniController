#ifndef SERIAL_SRV_H
#define SERIAL_SRV_H

/*! \file   serial_srv.h

    \brief  Header for serial port driver
    
    
*/

#include "system.h"

#define SRV_SERIAL_UxART1_ID       0
#define SRV_SERIAL_UxART6_ID       1


#define SRV_SERIAL_DEBUG		   SRV_SERIAL_UxART1_ID


void    srv_serial_init(void);
void    srv_serial_once(void);


typedef  void ( * serial_rec_char)(uint32_t port_id,uint8_t,portBASE_TYPE *);


void    srv_serial_rcv_callback(uint32_t port_id,serial_rec_char);
void    srv_serial_send(uint32_t port_id,const char * buffer,int length);
void    srv_serial_puts(uint32_t port_id,const char * buffer,int length);
void    srv_serial_baudrate(uint32_t port_id,uint32_t baudrate);
void    srv_serial_gps_loop(uint32_t enable);



#endif // SERIAL_SRV_H
