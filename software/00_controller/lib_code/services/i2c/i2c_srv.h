/*! 

    \file  i2c_srv.h

    
*/


#ifndef I2C_SRV_H
#define I2C_SRV_H


#include "system.h"

#define I2C_IIDX_BASE   0


void     srv_i2c_init(void);
void     srv_i2c_once(void);

void     srv_i2c_configuration(uint32_t iidx);
void     srv_i2c_recovery(uint32_t iidx);

uint32_t srv_i2c_read_buffer(uint32_t iidx,uint8_t * pBuffer, uint16_t DevAddr,uint16_t ReadAddr, volatile uint32_t * NumByteToRead);
int      srv_i2c_write_buffer(uint32_t iidx,const uint8_t* pBuffer,uint16_t DevAddr, uint16_t WriteAddr, volatile uint32_t * NumByteToWrite);




#endif // I2C_SRV_H

