#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H
 
#include "system.h"
#include "motion_scurve.h"


#define  MF_BUFFER_CNT	1024
#define  MF_JOB_CNT		  32


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

typedef struct _motion_buffer_t
{
	int32_t						dir;
	uint64_t					accu;

	motion_pos_t				pos_beg;
	motion_pos_t				pos_end;
	motion_phases_e				phase;

	uint32_t		    		pre_delay;
	uint32_t		    		post_delay;
	motion_phase_t  			mf[MF_PHASES_CNT];

	struct _motion_buffer_t  *  next;
}motion_buffer_t;


typedef struct
{
	uint32_t			head;
	uint32_t			tail;
}motion_cbuf_t;


typedef struct
{
	uint32_t			motion_type;
	uint32_t			motion_flags;

	motion_cbuf_t		mb_alloc;

	motion_buffer_t   *	mb_axis[AXIS_CNT];
}motion_job_t;



void 	motion_engine_init(void);
void 	motion_engine_once(void);


int32_t motion_engine_job_init(motion_job_t ** mj);

int32_t motion_engine_job_start(motion_job_t * mj);
int32_t motion_engine_job_abort(motion_job_t * mj);



void 	motion_engine_run(motion_job_t * mj,uint32_t axis_idx,int32_t pos_001mm,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3);
void 	motion_engine_run_home(motion_job_t * mj,uint32_t axis_idx,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3);

void 	motion_engine_stop(uint32_t axis_mask,uint32_t abort_mask);


uint32_t motion_engine_run_status(void);
void 	 motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc);


#endif // MOTION_ENGINE_H
