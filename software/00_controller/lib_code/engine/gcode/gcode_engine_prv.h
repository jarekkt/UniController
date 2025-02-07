#ifndef GCODE_ENGINE_MOTION_PRV_H
#define GCODE_ENGINE_MOTION_PRV_H
 
#include "gcode_parser.h"

typedef struct
{
	int32_t		  			is_inch;
	uint32_t		  		is_incremental;
	path_params_t		 	path_param;
}gcode_ctx_t;

typedef struct
{
	gcode_ctx_t  			 gcx;
}gcode_data_t;


typedef struct
{
	int32_t dummy;
}gcode_nv_data_t;


extern gcode_data_t     gcd;




#endif // GCODE_ENGINE_MOTION_PRV_H
