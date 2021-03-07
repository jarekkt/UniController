/*!
    \file motion_engine.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "motion_engine.h"
#include "motion_scurve.h"





typedef struct
{
	uint32_t 			step_freq;

}motion_nv_data_t;


/* Main motion buffers */

static motion_buffer_t   mb_global[MF_BUFFER_CNT];
static motion_job_t		 mj_global[MF_JOB_CNT];


motion_ctx_t 	  mctx;
motion_nv_data_t  mctx_nv VAR_NV_ATTR;


//static void motion_engine_tmr_endpos(void);
static void motion_task(void * params);

void motion_engine_init_default(void)
{
	memset(&mctx_nv,0,sizeof(mctx_nv));

	mctx_nv.step_freq = 100000;
}

void motion_engine_init(void)
{
	memset(&mctx,0,sizeof(mctx));
	srv_nov_register(&mctx_nv,sizeof(mctx_nv),motion_engine_init_default);
}

void motion_engine_reconfig(void)
{
	// Pulse length calculation - step frequency to 0.1us step pulse units
	srv_timer_pulse_period( (10*1000000 / mctx_nv.step_freq)/2);

	// Step calculations generation
	srv_timer_callback_step(mctx_nv.step_freq,motion_engine_tmr_step);
}

void motion_engine_once(void)
{
	srv_timer_callback_fast_add(motion_engine_tmr_endpos);

	motion_scurve_test_all();

	vSemaphoreCreateBinary(mctx.motion_kick);
	xTaskCreate( motion_task, "Motion", 6 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 4, NULL );
}


int32_t motion_engine_job_init(motion_job_t ** mj)
{
	int32_t result = -1;

	uint32_t new_mj_g_head;
	uint32_t ii;

	new_mj_g_head = (mctx.mj_g_head + 1)%MF_JOB_CNT;
	if(new_mj_g_head != mctx.mj_g_tail)
	{
		*mj = &mj_global[mctx.mj_g_head];
		memset(*mj,0,sizeof(**mj));
		mctx.mj_g_head = new_mj_g_head;

		// Initialize positions
		for(ii =0; ii< AXIS_CNT;ii++)
		{
			(*mj)->pos_beg001mm[ii] = (*mj)->pos_end001mm[ii] = mctx.plan_pos001mm[ii];
		}
		result = 0;
	}

	return result;
}

void motion_engine_jobs_start()
{
	// Advance with new jobs
	mctx.mj_g_run_head = mctx.mj_g_head;
}

void motion_engine_jobs_abort()
{
	int32_t ii;

	uint32_t mj_g_run_head = mctx.mj_g_run_head;


	// Restore planned position
	if(mj_g_run_head != mctx.mj_g_head)
	{
		for(ii =0; ii< AXIS_CNT;ii++)
		{
			mctx.plan_pos001mm[ii]  =  mj_global[mj_g_run_head].pos_beg001mm[ii];
		}
	}

	// Restore buffers used
	while(mj_g_run_head != mctx.mj_g_head)
	{
		if( mj_global[mj_g_run_head].mb_head != mj_global[mj_g_run_head].mb_tail)
		{
			mctx.mb_g_head = mj_global[mj_g_run_head].mb_tail;

			break;
		}
		mj_g_run_head = (mj_g_run_head + 1)%MF_JOB_CNT;
	}

	// Remove configured moves
	mctx.mj_g_head = mctx.mj_g_run_head;
}



void motion_engine_stop(uint32_t abort)
{

}





int32_t motion_engine_run_home
(	motion_job_t * mj,
	uint32_t axis_idx,
	int32_t speed_001mm_s,
	int32_t accel_001mm_s2,
	int32_t jerk_001mm_s3
)
{
	int32_t dist;

	if(axis_idx >AXIS_CNT)
	{
		fw_assert(0);
		return -1;
	}

	mctx.home_axis		 |= (1<<axis_idx);


	// Calculate max possible distance, add 20% to travel range
	dist = 12 *(ppctx_nv->axis[axis_idx].endpos_max_value - ppctx_nv->axis[axis_idx].endpos_min_value) / 10;

	if(speed_001mm_s < 0)
	{
		dist 				= -dist;
		speed_001mm_s		= -speed_001mm_s;
		mctx.home_axis_dir &= ~(1<<axis_idx);
	}
	else
	{
		mctx.home_axis_dir |= (1<<axis_idx);
	}

	return motion_engine_run(mj,axis_idx,dist,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);
}


void motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc)
{
	motion_buffer_t  mb;

	memset(&mb,0,sizeof(mb));
	motion_engine_convert(0,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[0],&mb);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(1,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[1],&mb);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(1,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[2],&mb);
}


int32_t  motion_engine_run(motion_job_t * mj,uint32_t axis_idx,int32_t pos_001mm,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3)
{
	motion_calc_t  		calc;
	int32_t		   		dist_001mm;
	int32_t		   		curr_pos001mm;
	uint32_t       		new_mb_g_head;
	motion_buffer_t	  * mb;


	// Calculate relative move
	curr_pos001mm = mctx.plan_pos001mm[axis_idx];
	dist_001mm	  = curr_pos001mm - pos_001mm;

	// Update new target position
	mctx.plan_pos001mm[axis_idx] = pos_001mm;


	// Get motion buffer for the move
	if(mj->mb_head != mj->mb_tail)
	{
		// We already have some buffers
	}
	else
	{
		// No buffers yet
		mj->mb_head = mj->mb_tail = mctx.mb_g_head;
	}

	new_mb_g_head = (mctx.mb_g_head + 1)% MF_BUFFER_CNT;

	if(new_mb_g_head != mctx.mb_g_tail)
	{
		mb = &mb_global[mctx.mb_g_head];

		memset(mb,0,sizeof(*mb));

		mj->mb_head 	= new_mb_g_head;
		mctx.mb_g_head = new_mb_g_head;
	}
	else
	{
		return -1;
	}

	// Calculate 3rd order S curve
	motion_scurve_calc(&calc,dist_001mm, ppctx_nv->axis[axis_idx].speed_safe_001mm_s,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);

	// Convert calculation for step engine format
	motion_engine_convert(axis_idx,dist_001mm,pos_001mm,mctx_nv.step_freq,&calc,&ppctx_nv->axis[axis_idx],mb);

	// Attach motion to buffers
	if(mj->mb_axis_head[axis_idx] == NULL)
	{
		// Empty list
		mj->mb_axis_head[axis_idx] = mb;
		mj->mb_axis_tail[axis_idx] = mb;
	}
	else
	{
		// Attach new motion to the end of link list
		mj->mb_axis_tail[axis_idx]->next = mb;
		mj->mb_axis_tail[axis_idx] = mb;
	}

	mctx.plan_pos001mm[axis_idx] = pos_001mm;

	return 0;
}





uint32_t motion_engine_run_status(void)
{
	//return mctx.mt_active;
	return 0;
}



static void motion_task(void * params)
{
	while(1)
	{
		xSemaphoreTake(mctx.motion_kick,portMAX_DELAY);




	}
}







#if 0
void motion_engine_tmr_masks()
{
	int ii;

	mctx.mt_active   &= mctx.abort_mask;
	mctx.mt_active   |= mctx.start_mask;
	mctx.abort_mask  = 0;
	mctx.start_mask  = 0;

	if(mctx.stop_mask != 0)
	{
		for(ii = 0; ii < AXIS_CNT;ii++)
		{
			if(mctx.mt_active & (mctx.stop_mask & (1<<ii)))
			{
#if 0
				switch(mctx.mt[ii].phase)
				{
					case MF_START_CONCAVE:
					{

					}break;

					case MF_START_LINEAR:
					{

					}break;

					case MF_START_CONVEX:
					{

					}break;

					case MF_CONSTANT_SPEED:
					{

					}break;

					case MF_STOP_CONVEX:
					case MF_STOP_LINEAR:
					case MF_STOP_CONCAVE:
					case MF_PHASES_CNT:
					{
						// Already in stopping phase
					}break;
				}
#endif
			}
		}
		mctx.stop_mask = 0;
	}
}
#endif

void motion_engine_tmr_step(void)
{

}


void motion_engine_tmr_endpos(void)
{

#if 0
	uint32_t inputs;
	uint32_t is_hit = 0;
	uint32_t ii;

	mctx.encoders[0] = srv_timer_quad_get(0);
	mctx.encoders[1] = srv_timer_quad_get(1);

	if(mctx.mt_active == 0)
	{
		mctx.endpos_hit_cntr = 0;
		return;
	}

	inputs = srv_gpio_get_io();


	for(ii =0; ii < 3;ii++)
	{
		if(mctx.mt_active & (1<<ii))
		{
			if(&mctx.mt[ii].dir == 0)
			{
				if( (inputs & ppctx_nv->axis[ii].endpos_max_mask) != 0)
				{
					is_hit = 1;
				}
			}
			else
			{
				if( (inputs & ppctx_nv->axis[ii].endpos_min_mask) != 0)
				{
					is_hit = 1;
				}
			}
		}
	}


	if( (inputs & ppctx_nv->estop_mask )!= 0)
	{
		is_hit = 1;
	}

	if(is_hit != 0)
	{
		if(mctx.endpos_hit_cntr < HIT_LIMIT)
		{
			mctx.endpos_hit_cntr++;
		}
		else
		{
			mctx.abort_mask |= mctx.mt_active;
			mctx.irq_mask 	= 1;
		}
	}
	else
	{
		if(mctx.endpos_hit_cntr >0)
		{
			mctx.endpos_hit_cntr--;
		}
	}
#endif
}


