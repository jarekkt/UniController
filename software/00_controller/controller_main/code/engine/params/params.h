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
	AXIS_FAST_CNT,
	AXIS_A = AXIS_FAST_CNT,
	AXIS_B,
	AXIS_C,
	AXIS_D,
	AXIS_GLOBAL_CNT
}axis_idx_e;

typedef struct
{
	float   speed_mm_s;
	float   accel_mm_s2;
	float   jerk_mm_s3;
}path_params_t;


typedef struct
{
	uint32_t  pulses_step_m;
	uint32_t  pulses_enc_m;

	uint32_t  endpos_min_mask;
	uint32_t  endpos_max_mask;

	float     endpos_min_value;
	float     endpos_max_value;

	float     speed_mm_s;
	float     speed_home_mm_s;
	float     speed_safe_mm_s;

	float     accel_mm_s2;
	float     jerk_mm_s3;

}axis_params_t;



typedef struct
{
	axis_params_t 		axis[AXIS_GLOBAL_CNT];
	path_params_t		path_initial_m;
	uint32_t			estop_mask;
	uint32_t			axis_used_mask;
}params_nv_ctx_t;



extern const params_nv_ctx_t  *  ppctx_nv;



#endif // PARAMS_H
