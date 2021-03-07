#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H
 
#include "system.h"
#include "engine.h"
#include "services.h"
#include "motion_scurve.h"




#define  MF_BUFFER_CNT	1024
#define  MF_JOB_CNT		 256


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
	int32_t				dir;
	uint64_t			accu;

	motion_pos_t		pos_beg;
	motion_pos_t		pos_end;

	motion_phases_e		phase;

	uint32_t		    pre_delay;
	uint32_t		    post_delay;
	motion_phase_t  	mf[MF_PHASES_CNT];

	struct _motion_buffer_t  *  next;
}motion_buffer_t;



typedef struct
{
	uint32_t			motion_type;
	uint32_t			motion_flags;

	int32_t				pos_beg001mm[AXIS_CNT];
	int32_t				pos_end001mm[AXIS_CNT];

	uint32_t 			mb_tail;
	uint32_t 			mb_head;

	motion_buffer_t   *	mb_axis_head[AXIS_CNT];
	motion_buffer_t   *	mb_axis_tail[AXIS_CNT];
}motion_job_t;


typedef struct
{
	xSemaphoreHandle    motion_kick;

	/* Special case - homing variables */
	uint32_t			home_axis;
	uint32_t			home_axis_dir;

	uint32_t			endpos_hit_cntr;
	int32_t				encoders[2];

	/* Buffer management - motion profiles */
	uint32_t			mb_g_tail;
	uint32_t		    mb_g_head;

	/* Buffer management - job management */
	uint32_t 			mj_g_tail;
	uint32_t 			mj_g_run;
	uint32_t 			mj_g_run_head;
	uint32_t 			mj_g_head;

	/* Pulse position (current) */
	int32_t 			curr_pulse_pos[AXIS_CNT];

	/* Position - future */
	int32_t	     		plan_pos001mm[AXIS_CNT];


}motion_ctx_t;



void 	motion_engine_init(void);
void 	motion_engine_once(void);

int32_t motion_engine_job_init(motion_job_t ** mj);
void	motion_engine_jobs_start();
void	motion_engine_jobs_abort();


int32_t motion_engine_run
(			motion_job_t * mj,
			uint32_t axis_idx,
			int32_t pos_001mm,
			int32_t speed_001mm_s,
			int32_t accel_001mm_s2,
			int32_t jerk_001mm_s3
);

int32_t  motion_engine_run_home
(			motion_job_t * mj,
			uint32_t axis_idx,
			int32_t speed_001mm_s,
			int32_t accel_001mm_s2,
			int32_t jerk_001mm_s3
);

void 	 motion_engine_stop(uint32_t abort);

uint32_t motion_engine_run_status(void);
void 	 motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc);
void 	 motion_engine_tmr_step(void);
void 	 motion_engine_tmr_endpos(void);


// motion_engine_calc.c
void 	motion_engine_convert
(			uint32_t axis_idx,
			int32_t dist_001mm,
			int32_t pos_001mm,
			uint32_t step_freq,
			const motion_calc_t  * calc,
			const axis_params_t * axis,
			motion_buffer_t * mbfr
);

// motion_engine_pins.c
void 	motion_engine_dir(int32_t idx,int32_t dir);
int32_t motion_engine_tmr_pins
(			int32_t * pulse_pos,
			motion_buffer_t * mbx,
			motion_buffer_t * mby,
			motion_buffer_t * mbz
);







#endif // MOTION_ENGINE_H
