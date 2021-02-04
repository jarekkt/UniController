#ifndef NOV_SRV_H
#define NOV_SRV_H

/*! \file   nov_srv.h

    \brief  Header for low level NOV support
    
    
*/




#define VAR_NV_ATTR     	  __attribute__ ((section (".var_nv")))
#define VAR_NV_CRC_ATTR       __attribute__ ((section (".var_nv_crc")))
#define VAR_SFLASH_ATTR  	  __attribute__ ((section(".sflash")))






extern uint8_t __var_nv_start__[];
extern uint8_t __var_nv_end__[];

#define 	__var_nv_size__    (__var_nv_end__ - __var_nv_start__)




/*
    Note - not to be used directly - only through calls specified in  tsk_storage.h

*/

typedef void (*srv_printf_t) (uint32_t level_mask,const char * format, ...);


int   srv_nov_init(void);
void  srv_nov_init_default(int mode);
void  srv_nov_print_info(srv_printf_t pprintf);
int   srv_nov_is_changed(void);
int   srv_nov_store(void);
void  srv_nov_start(void);
void  srv_nov_register(void * data, size_t size, void (*default_fn)(void));







void  srv_nov_dump_change(srv_printf_t pprintf);

#endif //NOV_SRV_H


