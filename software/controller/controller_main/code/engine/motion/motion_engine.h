#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H
 
#include "system.h"
#include "motion_scurve.h"

typedef enum
{
	MF_START_CONCAVE,
	MF_START_LINEAR,
	MF_START_CONVEX,
	MF_CONSTANT_SPEED,
	MF_STOP_CONVEX,
	MF_STOP_LINEAR,
	MF_STOP_CONCAVE,
	MF_PHASES_CNT
}motion_phases_e;

typedef struct
{
	uint32_t			pulse_count_total;
	uint32_t			pulse_count;

	uint64_t			speed_fract;
	int64_t	    		accel_fract;
	int64_t				jerk_fract;
}motion_phase_t;

typedef struct
{
	int32_t     		pulse_count;
	int32_t     		enc_count;
	int32_t				pos_001mm;
}motion_pos_t;


typedef struct
{
	uint64_t			accu;

	motion_pos_t		pos_cur;
	motion_pos_t		pos_next;

	int32_t				dir;
	motion_phases_e		phase;
	motion_phase_t  	mf[MF_PHASES_CNT];
}motion_timer_t;




void 	motion_engine_init(void);
void 	motion_engine_once(void);


void 	motion_engine_run(uint32_t axis_idx,int32_t pos_001mm,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3);
void 	motion_engine_run_home(uint32_t axis_idx,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3);
void 	motion_engine_stop(uint32_t axis_mask,uint32_t abort_mask);
uint32_t motion_engine_run_status(void);

void 	motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc);


#endif // MOTION_ENGINE_H
