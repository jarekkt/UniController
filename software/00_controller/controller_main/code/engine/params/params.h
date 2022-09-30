#ifndef PARAMS_H
#define PARAMS_H
 
#include "system.h"
#include "common.h"


void params_init(void);
void params_once(void);

#define MM_TO_001(x_)   ((uint32_t)((x_)*1000))

#define P_HOMING_NONE			0
#define P_HOMING_TO_MIN			1
#define P_HOMING_TO_MAX			2
#define P_HOMING_TO_MID			3

#define P_IN_CPU1				BV(0)
#define P_IN_CPU2				BV(1)
#define P_IN_CPU3				BV(2)
#define P_IN_CPU4				BV(3)
#define P_IN_CPU5				BV(4)
#define P_IN_CPU6				BV(5)
#define P_IN_CPU7				BV(6)
#define P_IN_CPU8				BV(7)
#define P_IN_CPU9				BV(8)
#define P_IN_CPU10				BV(9)


#define P_IO_CPU1				BV(16)
#define P_IO_CPU2				BV(17)
#define P_IO_CPU3				BV(18)
#define P_IO_CPU4				BV(19)
#define P_IO_CPU5				BV(20)
#define P_IO_CPU6				BV(21)
#define P_IO_CPU7				BV(22)
#define P_IO_CPU8				BV(23)
#define P_IO_CPU9				BV(24)




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

	uint32_t  homing_type;
	uint32_t  homing_mask;
	float     homing_value;
	float     homing_retract_mm;

	float     speed_mm_s;
	float     speed_home_mm_s;
	float     speed_home_retract_mm_s;

	float     speed_safe_mm_s;

	float     accel_mm_s2;
	float     jerk_mm_s3;

}axis_params_t;

typedef struct
{
	uint32_t min;
	uint32_t max;
	uint32_t delay_ms;
}pwm_range_t;


typedef struct
{
	axis_params_t 		axis[AXIS_GLOBAL_CNT];
	path_params_t		path_initial_m;
	uint32_t			io_rev_mask;
	uint32_t			dir_rev_mask;
	uint32_t			estop_mask;
	uint32_t			axis_used_mask;
	uint32_t			io_cpu_dir;
	uint32_t			in_cpu_pullup;
	pwm_range_t			pwm_range[PWM_GLOBAL_CNT];
}params_nv_ctx_t;



extern const params_nv_ctx_t  *  ppctx_nv;


void	params_chm36va(void);
void	params_dump();



#endif // PARAMS_H
