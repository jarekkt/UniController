#ifndef PARAMS_H
#define PARAMS_H
 
#include "system.h"

void params_init(void);
void params_once(void);

#define MM_TO_001(x_)   ((uint32_t)((x_)*1000))

typedef enum
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_CNT
}axis_idx_e;

typedef struct
{
	int32_t   pulses_step_100mm;
	int32_t   pulses_enc_100mm;

	uint32_t  endpos_min_mask;
	uint32_t  endpos_park_mask;
	uint32_t  endpos_max_mask;

	int32_t   endpos_min_value;
	uint32_t  endpos_park_value;
	int32_t   endpos_max_value;

	int32_t   speed_001mm_s;
	int32_t   speed_home_001mm_s;
	uint32_t  speed_safe_001mm_s;

	int32_t   accel_001mm_s2;
	int32_t   jerk_001mm_s3;

}axis_params_t;



typedef struct
{
	axis_params_t 		axis[AXIS_CNT];
	uint32_t			estop_mask;
	uint32_t 			dupa;
}params_nv_ctx_t;



extern const params_nv_ctx_t  *  ppctx_nv;



#endif // PARAMS_H
