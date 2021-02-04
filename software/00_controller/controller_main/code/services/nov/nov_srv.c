/*! \file   srv_nov.c

    \brief  Low level NOV support
    
    
*/ 

#include "system.h" 
#include "services.h"
#include "middleware.h"
#include "nov_srv_qspi.h"
 
#define NV_SHADOW_BUFFER_SIZE  2048

#define STORE_FIRST_COPY_1 0
#define STORE_FIRST_COPY_2 1


typedef void (* default_fn)(void);

typedef struct
{
	default_fn  default_functions[64];
	uint32_t    default_functions_cnt;
	uint32_t    done;
	uint32_t	store_first;
	uint32_t    state;
}nov_ctx_t;

typedef struct
{
    uint32_t nvCounter;
    uint32_t serial_nr;
}srv_nov_nv_t;


srv_nov_nv_t     srv_nov_nv  VAR_NV_ATTR;
nov_ctx_t		 nctx = { 0 };

uint8_t 		 flash_storage[2][QSPI_SECTOR_SIZE] VAR_SFLASH_ATTR;
uint8_t			 flash_storage_buffer[QSPI_SECTOR_SIZE];


static int32_t srv_nov_store_copy(uint32_t copy_idx);


/*! Table of CRC values for highorder byte */
static uint8_t auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/*! Table of CRC values for loworder byte */
static uint8_t auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;




/*!

    \brief  Standard CRC16 routine

    \param [in] puchMsg character stream
    \param [in] usDataLen stream length

    \retval CRC16 value of given stream

    
*/


static uint16_t crc16(const uint8_t *puchMsg,uint16_t usDataLen)
{
    uint8_t uchCRCHi = 0xFF ; /* high byte of CRC initialized */
    uint8_t uchCRCLo = 0xFF ; /* low byte of CRC initialized */
    int32_t uIndex ;             /* will index into CRC lookup table */
    
    while (usDataLen--)
    {
        uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}



/*!

    \brief  Reads NOV storage from EEPROM


    \retval 0 if succesfull NOV retrival was performed

    Note: Test mode is not preserved across resets (test_mode variable is always cleared).
          Emulation mode however persists ( if, after enabling emulation, the reset was executed by serial command)


    \page nov  Non volatile storage

    The Nov block is responsible for non volatile storage of device data.
    There are two types of data Nov block stores in EEPROM:
    - Device parameters
        - The parameters are stored in two identical copies. 
            - During startup, first copy is read and checked for proper checksum ( crc 16). If valid, these parameters are used.
              If the copy is not valid, second copy is used. If this one is faulty - the it is fatal error, default values are used.
            - When write request arrives, all parameters are written at once. Starting from second copy, then the first copy 
              ( so there is guarantee always one copy is valid even if power cuts in random moment ). 
              We start from second copy as we know that first one was valid (we do not have such guarantee about second) 


              
*/

int  srv_nov_read_flash(void)
{

    uint8_t              result = 1;
    uint16_t             crc;
    uint16_t             r_crc;
    uint32_t             valid_copy = 0;


    fw_assert(sizeof(flash_storage[0]) > __var_nv_size__ + sizeof(uint16_t) );
     
    
    nctx.state = 0;
   
    // Try first copy
    crc      = crc16((const unsigned char*)flash_storage[0],__var_nv_size__);
    memcpy(&r_crc,&flash_storage[0][__var_nv_size__],sizeof(r_crc));

    if(crc == r_crc)
    {
        // Copy valid
        memcpy(__var_nv_start__,flash_storage[0],__var_nv_size__);
        result = 0;
        nctx.state |= 0x01;

        valid_copy++;
    }
    else
    {
        nctx.state |= 0x02;
    }


    // Try backup copy
    crc   = crc16((const unsigned char*)flash_storage[1],__var_nv_size__);
    memcpy(&r_crc,&flash_storage[1][__var_nv_size__],sizeof(r_crc));

    if(crc == r_crc)
    {
        nctx.state |= 0x04;
        valid_copy++;

        if(result != 0)
        {
            nctx.state |= 0x08;
            // Only second copy valid - restore first copy to guarantee that the first one is always valid
            memcpy(__var_nv_start__,flash_storage[1],__var_nv_size__);
            nctx.store_first = STORE_FIRST_COPY_1;
            result      = 0;
        }
    }
    else
    {

        nctx.state |= 0x10;
        if(result == 0)
        {
            nctx.state |= 0x20;
            // First was valid, second corrupted. Restore second.
            nctx.store_first = STORE_FIRST_COPY_2;
        }        
    }

    return result;    
}


void srv_nov_init_default(int mode)
{
    uint32_t  sn;
    uint32_t  nvCounter;

    if( (mode & 0x01) == 0) 
    {
          /* Not a fresh invalid NV storage */  
          sn        = srv_nov_nv.serial_nr;
          nvCounter = srv_nov_nv.nvCounter;      
    }
    else
    {
          /* Unknown previous state */
          sn          = 0;
          nvCounter   = 0;
    }


    memset(&srv_nov_nv,0,sizeof(srv_nov_nv));


    // Restore some vital variables
    srv_nov_nv.serial_nr = sn;
    srv_nov_nv.nvCounter = nvCounter;  

}

int  srv_nov_init(void)
{
   int status;
   int ii;


   srv_nov_qspi_init();

   status = srv_nov_read_flash();

   if(status != 0)
   {
      for(ii = 0; ii < nctx.default_functions_cnt;ii++)
      {
    	  nctx.default_functions[ii]();
      }
      srv_nov_init_default(1);
      srv_nov_store();

   }

   nctx.done = 1;

   return status;
}




/*!

    \brief  Function which prints nov status - acquired when logging was not available yet


    \param  state  Status set by #srv_nov_init()
*/    


void  srv_nov_print_info(srv_printf_t pprintf)
{
    if(nctx.state& 0x01)(*pprintf)(1,"nov: first  copy OK\r\n");
    if(nctx.state& 0x02)(*pprintf)(1,"nov: first  copy faulty\r\n");
    if(nctx.state& 0x04)(*pprintf)(1,"nov: second copy OK\r\n");
    if(nctx.state& 0x08)(*pprintf)(1,"nov: first  copy recovery\r\n");
    if(nctx.state& 0x10)(*pprintf)(1,"nov: second copy faulty\r\n");
    if(nctx.state& 0x20)(*pprintf)(1,"nov: second copy recovery\r\n");
}



static int32_t srv_nov_store_copy(uint32_t copy_idx)
{
    int32_t					result = 0;
    uint16_t   				srv_nov_crc;



    if(copy_idx > 1)
    {
    	return -1;
    }

     /*!
    
        \wdg  Watchdog kicks - Before each NOV storage operations - once, as erase may be 
              relatively ( few miliseconds).
                           
     */
     
    srv_wdg_kick();



    /*!
            \tst  Variable 'nvCounter' Incremented each time NV storage contents is written to the flash.
                                       Note that it is written for each NV copy, so each update will increment it by factor 2.        
    */ 

    srv_nov_nv.nvCounter ++;
    memcpy(flash_storage_buffer,__var_nv_start__,__var_nv_size__);

    srv_nov_crc = crc16(__var_nv_start__,__var_nv_size__);
    memcpy(&flash_storage_buffer[__var_nv_size__],&srv_nov_crc,sizeof(srv_nov_crc));
    
    result = srv_nov_qspi_write(flash_storage_buffer,__var_nv_size__ + sizeof(uint16_t) ,flash_storage[copy_idx]-QSPI_START);


    /*!
               
          \wdg Watchdog kick - after NOVoperation                                                      
    */
     
    srv_wdg_kick();

    return result;


    return 0;
}


/*!

    \brief  Writes NOV storage to EEPROM

    \retval  O in case of success

    Note: Updates both copies - one after another    
*/

int srv_nov_store(void)
{
    int result;

    if(nctx.store_first ==  STORE_FIRST_COPY_1)
    {
        result = srv_nov_store_copy(0);

        if(result == 0)
        {
            result = srv_nov_store_copy(1);
        }
    }
    else
    {
        result = srv_nov_store_copy(1);

        if(result == 0)
        {
            result = srv_nov_store_copy(0);
        }
    }   



    return result;
}


int   srv_nov_is_changed(void)
{
    return memcmp(__var_nv_start__,flash_storage[0],__var_nv_size__);
}


void  srv_nov_register(void * data, size_t size, void (*default_fn)(void))
{
    if(default_fn != NULL)
    {
      if(nctx.default_functions_cnt < DIM(nctx.default_functions))
      {
    	  nctx.default_functions[nctx.default_functions_cnt++] = default_fn;
      }
      else
      {
        fw_assert(0);
      }
    }

    fw_assert(nctx.done == 0);
}



//Unused on target platform
void srv_nov_start()
{

}

