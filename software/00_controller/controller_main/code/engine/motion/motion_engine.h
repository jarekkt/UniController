#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H
 
#include "system.h"
#include "engine.h"
#include "services.h"
#include "motion_scurve.h"




#define  MF_BUFFER_CNT	4096
#define  MF_JOB_CNT		 256

#define  MF_FLAG_RUNNING 0x0001
#define  MF_FLAG_DONE	 0x0002

#define  HIT_LIMIT		5

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

typedef enum
{
	JCMD_OK,
	JCMD_FAIL,
	JCMD_MOTION,
	JCMD_COORDS,
	JCMD_FWINFO,
	JCMD_SENSORS,
	JCMD_OUTPUTS
}jcmd_e;

typedef struct
{
	uint32_t			pulse_count_total;
	uint32_t			pulse_count;

	uint32_t			tick_delay;

	uint64_t			speed_fract;
	int64_t	    		accel_fract;
	int64_t				jerk_fract;

	uint64_t		    accu;
}motion_phase_t;


typedef struct
{
	int32_t     		pulse_count;
	int32_t     		enc_count;
	float				pos_mm;
}motion_pos_t;



typedef struct _motion_buffer_t
{
	int32_t				dir;
	motion_phase_t  	mf;

	struct _motion_buffer_t  *  next;
}motion_buffer_t;



typedef struct
{
	jcmd_e			jcmd;

	struct
	{
		uint32_t		home_axis;
		uint32_t		home_axis_mask;
		uint32_t		cmd_args[2];
	}args;

	uint32_t			task_flags;

	float				pos_beg_mm[AXIS_CNT];
	float				pos_end_mm[AXIS_CNT];

	uint32_t 			mb_tail;
	uint32_t 			mb_head;

	burst_rcv_ctx_t		comm_ctx;

	motion_buffer_t   *	mb_axis_head[AXIS_CNT];
	motion_buffer_t   *	mb_axis_tail[AXIS_CNT];
}motion_job_t;


typedef struct
{
	xSemaphoreHandle    motion_kick;

	/* Request commands */
	uint32_t 			req_stop;

	/* Buffer management - motion profiles */
	uint32_t			mb_g_tail;
	uint32_t		    mb_g_head;

	/* Buffer management - job management */
	uint32_t 			mj_g_tail;
	uint32_t 			mj_g_run_head;
	uint32_t 			mj_g_head;

	/* Pulse position (current) */
	int32_t 			curr_pulse_pos[AXIS_CNT];
	int32_t				curr_dir[AXIS_CNT];
	int32_t				active_dir[AXIS_CNT];


	/* Position - future */
	float	     		plan_pos_mm[AXIS_CNT];
	float	     		offset_pos_mm[AXIS_CNT];

	/* Flow control */
	uint32_t		    hit_active;
	uint32_t		    hit_mask;
	uint32_t			endpos_hit_cntr;
	uint32_t 			stop_active;

	/* Current job */
	motion_job_t	 * job;

	/* Command response buffer */
	char			    resp_buffer[256];

}motion_ctx_t;



void 	motion_engine_init(void);
void 	motion_engine_once(void);

int32_t motion_engine_job_init(motion_job_t ** mj,const burst_rcv_ctx_t * comm_ctx);
void    motion_engine_ack(motion_job_t * mj,int32_t result);
void	motion_engine_jobs_start();
void	motion_engine_jobs_abort();


int32_t motion_engine_run
(			motion_job_t * mj,
			uint32_t axis_idx,
			float pos_mm,
			float speed_mm_s,
			float accel_mm_s2,
			float jerk_mm_s3
);

int32_t  motion_engine_run_home
(			motion_job_t * mj,
			uint32_t axis_idx,
			float speed_mm_s,
			float accel_mm_s2,
			float jerk_mm_s3
);

void 	 motion_engine_stop(uint32_t abort);


void 	 motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc);
void 	 motion_engine_tmr_step(void);
void 	 motion_engine_tmr_endpos(void);


// motion_engine_calc.c
float    motion_engine_pulse_to_units(int32_t pulses,int32_t axis);
int32_t  motion_engine_units_to_pulse(float units_mm,int32_t axis);
int32_t  motion_engine_units_to_enc(float units_mm,int32_t axis);


int32_t	motion_engine_convert
(			uint32_t 				axis_idx,
			float 	    			from_pos_mm,
			float   				to_pos_mm,
			uint32_t 			    step_freq,
			const motion_calc_t   * calc,
			const axis_params_t   * axis,
			motion_buffer_t       * mbfr,
			int32_t				    mbfr_cnt
);

// motion_engine_pins.c
void 	 motion_engine_dir(int32_t idx,int32_t dir,int32_t * active_dir);
uint32_t motion_engine_step_axis(
			int32_t					axis_idx,
			motion_buffer_t 	 ** p_mbfr,
			int32_t 		      * pulse_pos,
			int32_t				  * dir,
			int32_t				  * active_dir,
			int32_t 		 	  * active
);







#endif // MOTION_ENGINE_H
