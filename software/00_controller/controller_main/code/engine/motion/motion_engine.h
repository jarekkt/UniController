#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H
 
#include "system.h"
#include "engine.h"
#include "services.h"
#include "motion_scurve.h"





#define  MF_JOB_CNT		300

#define  MF_BUFFER_CNT	(MF_JOB_CNT * 2 * 7)

#define  MF_FLAG_RUNNING 0x0001
#define  MF_FLAG_DONE	 0x0002

#define  MF_HOME_RUN	 0x0001
#define  MF_HOME_LAST	 0x0002



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
	JCMD_MOTION_HOME,
	JCMD_MOTION_HOME_ACK,
	JCMD_MOTION_WAIT,
	JCMD_COORDS,
	JCMD_FWINFO,
	JCMD_SENSORS,
	JCMD_OUTPUTS
}jcmd_e;


typedef double  engval_t;

typedef struct
{
	uint32_t			pulse_count_total;
	uint32_t			pulse_count;

	uint32_t			tick_delay;

	engval_t			speed_fract;
	engval_t	    	accel_fract;
	engval_t			jerk_fract;

	engval_t		    accu;
}motion_phase_t;





typedef struct
{
	int32_t     		pulse_count;
	int32_t     		enc_count;
	float				pos_mm;
}motion_pos_t;



typedef struct
{
	int32_t				dir;
	motion_phase_t  	mf;
}motion_buffer_data_t;




typedef struct _motion_buffer_t
{
	motion_buffer_data_t	    data;
	struct _motion_buffer_t  *  next;
}motion_buffer_t;


typedef struct
{
	uint32_t dev_status;

	uint32_t inputs;
	float    inputs_analog[2];

}motion_remote_status_t;


typedef struct
{
	uint32_t			home_phase;
	uint32_t			home_axis;
	float				home_value;
}motion_home_args_t;

typedef struct
{
	uint32_t			io_mask_endstop;
	uint32_t			io_mask_run_stop;
	uint32_t			io_mask_run_keep;
	uint32_t			io_mask_soft_max_stop;
	uint32_t			io_mask_soft_min_stop;
}motion_io_args_t;

typedef struct
{
	jcmd_e					jcmd;
	uint32_t				jcmd_args[2];



	motion_home_args_t		homing;
	motion_io_args_t		io;


	uint32_t				task_flags;

	float					pos_beg_mm[AXIS_GLOBAL_CNT];
	float					pos_end_mm[AXIS_GLOBAL_CNT];

	uint32_t 				mb_tail;
	uint32_t 				mb_head;

	burst_rcv_ctx_t			comm_ctx;

	motion_buffer_t   	*	mb_axis_head[AXIS_GLOBAL_CNT];
	motion_buffer_t   	*	mb_axis_tail[AXIS_GLOBAL_CNT];

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

	int32_t 			curr_pulse_pos[AXIS_GLOBAL_CNT];
	int32_t				curr_dir[AXIS_GLOBAL_CNT];
	int32_t				active_dir[AXIS_GLOBAL_CNT];

	/* Position (current) */

	float	     		offset_pos_mm[AXIS_GLOBAL_CNT];
	float				curr_pos[AXIS_GLOBAL_CNT];

	/* Position (future) */

	float	     		plan_pos_mm[AXIS_GLOBAL_CNT];

	/* Flow control */

	uint32_t		    hit_active;
	uint32_t		    hit_io;

	uint32_t		    stop_active;
	uint32_t			pulse_idle;

	/* Current job */
	motion_job_t	 * job;

	/* Command response buffer */
	char			    resp_buffer[384];

	/* Misc */

	uint32_t inputs;
	uint32_t inputs_filters[32];
	uint32_t inputs_filtered;


}motion_ctx_t;




void 	motion_engine_init(void);
void 	motion_engine_once(void);

int32_t motion_engine_job_init(motion_job_t ** mj,const burst_rcv_ctx_t * comm_ctx);
void    motion_engine_ack(motion_job_t * mj,int32_t result);
void	motion_engine_jobs_start();
void	motion_engine_jobs_abort();
void 	motion_engine_set_pos(
			int32_t	 axis_idx,
			float pos_mm
);


int32_t motion_engine_run
(			motion_job_t * mj,
			uint32_t axis_idx,
			float pos_mm,
			uint32_t is_incremental_mask,
			float speed_mm_s,
			float accel_mm_s2,
			float jerk_mm_s3
);

int32_t  motion_engine_run_home
(			uint32_t 					axis_idx,
			const burst_rcv_ctx_t     * rcv_ctx
);

int32_t motion_engine_delay
(			motion_job_t * mj,
			uint32_t	   delay_ms
);


void  	 motion_engine_offset_coords(float * axis,uint32_t axis_cnt);


void 	 motion_engine_stop(uint32_t abort);


void 	 motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc);
void 	 motion_engine_tmr_step(void);
void 	 motion_engine_tmr_endpos(void);


// motion_engine_calc.c
float    motion_engine_pulse_to_units(int32_t pulses,int32_t axis);
int32_t  motion_engine_units_to_pulse(float units_mm,int32_t axis);
int32_t  motion_engine_units_to_enc(float units_mm,int32_t axis);
void	 motion_engine_speed_test(void);


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



// motion_engine_io.c
int32_t  motion_engine_io(uint32_t pin,uint32_t value);
int32_t  motion_engine_io_analog(char * buffer,uint32_t length);





#endif // MOTION_ENGINE_H
