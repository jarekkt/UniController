/*!
    \file crc32.h
*/


#ifndef CRC32_H
#define CRC32_H

#include "system.h"

uint32_t  crc32(const char  *buf,uint32_t size);
uint32_t  crc32raw(uint32_t crc,const char  *buf,uint32_t size);
    

#endif
