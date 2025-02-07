#ifndef SERMON_SRV_H
#define SERMON_SRV_H



#include "system.h"


#ifndef _WIN32
#define SERMON_ATTR         __attribute__ ((section (".sermon"),used))
#else
#define SERMON_ATTR        
#endif


#define BURST_GUI_MASKS    14


typedef union
{
    struct
    {
        int32_t  min;
        int32_t  max;
    }range_I;

    struct
    {
        uint32_t  min;
        uint32_t  max;
    }range_U;

    struct
    {
        float  min;
        float  max;
    }range_F;


    
}var_range_t;



/*!
    Variable access flags
*/

typedef enum
{
    E_VA_INT_NONE = 0,
    E_VA_UINT_NONE,
    E_VA_FLT_NONE,
    E_VA_INT_FREE,
    E_VA_UINT_FREE,
    E_VA_FLT_FREE,
    E_VA_INT_LIMIT,
    E_VA_UINT_LIMIT,        
    E_VA_FLT_LIMIT        
   
}var_access_e;




typedef struct 
{
    char             name[SYS_VAR_NAME_LENGTH];
    void           * ptr;     
    uint32_t         access;    
    var_range_t      range;    
}var_ptable_t;





extern const char __sermon_start__[];
extern const char __sermon_end__[];


void srv_sermon_init(void); 
void srv_sermon_once(void);
void srv_sermon_init_default(void);
void srv_sermon_register(const var_ptable_t  table[],int cnt);




#endif        /* SRV_SERMON_H */
