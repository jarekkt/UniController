#ifndef NOV_SRV_H
#define NOV_SRV_H

/*! \file   nov_srv.h

    \brief  Header for low level NOV support
    
    
*/


#define VAR_NV_ATTR      __attribute__ ((section (".var_nv")))

extern char __var_nv_start__[];
extern char __var_nv_end__[];

extern char __var_nv_sh_start__[];
extern char __var_nv_sh_end__[];

#define __var_nv_size__ (__var_nv_end__ - __var_nv_start__)
#define __var_nv_sh_size__ (__var_nv_sh_end__ - __var_nv_sh_start__)



#define NOV_OVERWRITE_CALIB  0xBABA


/*
    Note - not to be used directly - only through calls specified in  tsk_storage.h

*/

typedef void (*srv_printf_t) (uint32_t level_mask,const char * format, ...);

void  srv_nov_pre_init(void);
int   srv_nov_init(void);
void  srv_nov_init_default(int mode);
void  srv_nov_print_info(srv_printf_t pprintf);
int   srv_nov_store(uint32_t destroy_mask);
int   srv_nov_is_changed(void);
void  srv_nov_register(void * data, size_t size,void (*default_fn)(void));
void  srv_nov_start(void);





void  srv_nov_dump_change(srv_printf_t pprintf);

#endif //NOV_SRV_H


