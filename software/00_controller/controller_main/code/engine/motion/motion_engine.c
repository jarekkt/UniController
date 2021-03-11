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


int32_t motion_engine_job_init(motion_job_t ** mj,const burst_rcv_ctx_t * comm_ctx)
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

		memcpy( &(*mj)->comm_ctx,comm_ctx,sizeof(*comm_ctx));

		result = 0;
	}

	return result;
}

void motion_engine_jobs_start()
{
	// Advance with new jobs

	mctx.mj_g_run_head = mctx.mj_g_head;
	xSemaphoreGive(mctx.motion_kick);

}

void motion_engine_stop(uint32_t abort)
{
	mctx.req_stop = 1 + abort;
	xSemaphoreGive(mctx.motion_kick);
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


int32_t motion_engine_run_home
(	motion_job_t * 	mj,
	uint32_t 		axis_idx,
	int32_t 		speed_001mm_s,
	int32_t 		accel_001mm_s2,
	int32_t 		jerk_001mm_s3
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
	motion_buffer_t  mb[7];

	memset(&mb,0,sizeof(mb));
	motion_engine_convert(0,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[0],mb,7);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(1,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[1],mb,7);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(1,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[2],mb,7);
}






int32_t  motion_engine_run(motion_job_t * mj,uint32_t axis_idx,int32_t pos_001mm,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3)
{
	motion_calc_t  		calc;
	int32_t		   		dist_001mm;
	int32_t		   		curr_pos001mm;
	uint32_t       		new_mb_g_head;
	uint32_t       		org_mb_g_head;
	int32_t				mb_used;
	int32_t				ii;
	motion_buffer_t  	mb[7];
	motion_buffer_t  *	mb_curr;



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


	// Calculate 3rd order S curve
	motion_scurve_calc(&calc,dist_001mm, ppctx_nv->axis[axis_idx].speed_safe_001mm_s,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);


	// Convert calculation for step engine format
	mb_used = motion_engine_convert(axis_idx,dist_001mm,pos_001mm,mctx_nv.step_freq,&calc,&ppctx_nv->axis[axis_idx],mb,DIM(mb));

	if(mb_used > 0)
	{
		// Inefficient way with copying - fix it later
		// TODO

		org_mb_g_head = mctx.mb_g_head;

		for( ii = 0; ii < mb_used;ii++)
		{
			// Allocate new buffer
			new_mb_g_head = (mctx.mb_g_head + 1)% MF_BUFFER_CNT;

			if(new_mb_g_head != mctx.mb_g_tail)
			{
				mb_curr = &mb_global[mctx.mb_g_head];

				memcpy(mb_curr,&mb[ii],sizeof(*mb_curr));

				mj->mb_head    = new_mb_g_head;
				mctx.mb_g_head = new_mb_g_head;
			}
			else
			{
				// Recover allocations
				mj->mb_head = org_mb_g_head;
				return -1;
			}

			// Attach motion to buffers
			if(mj->mb_axis_head[axis_idx] == NULL)
			{
				// Empty list
				mj->mb_axis_head[axis_idx] = mb_curr;
				mj->mb_axis_tail[axis_idx] = mb_curr;
				mb_curr->next = NULL;
			}
			else
			{
				// Attach new motion to the end of link list
				mj->mb_axis_tail[axis_idx]->next = mb_curr;
				mj->mb_axis_tail[axis_idx] 		 = mb_curr;
			}
		}
	}

	return 0;
}






static void motion_task_finish_job(uint32_t idx)
{
	//Currently nothing to do
}



static void motion_task_start_job(uint32_t idx)
{
	int32_t ii;

	for(ii =0; ii < AXIS_CNT;ii++)
	{
		// Process direction setup and changes
		//motion_engine_dir(ii,)
	}

}



static void motion_task(void * params)
{
	uint32_t	new_mj_g_tail;


	while(1)
	{
		xSemaphoreTake(mctx.motion_kick,portMAX_DELAY);

		// Check for control requests
		if(mctx.req_stop != 0)
		{
			// TODO

			mctx.req_stop = 0;
		}

		// First process finished jobs
		while( mctx.mj_g_tail != mctx.mj_g_run)
		{
			new_mj_g_tail = (mctx.mj_g_tail + 1)% MF_JOB_CNT;
			if(new_mj_g_tail != mctx.mj_g_run)
			{
				// Finished for sure
				motion_task_finish_job(new_mj_g_tail);
				mctx.mj_g_tail = new_mj_g_tail;
			}
			else
			{
				// Check if last task is still running
				if( ( mj_global[new_mj_g_tail].task_flags & MF_FLAG_DONE)!= 0)
				{
					motion_task_finish_job(new_mj_g_tail);
					mctx.mj_g_tail = new_mj_g_tail;
				}
			}
		}

		//Run new jobs if needed
		while(mctx.mj_g_run != mctx.mj_g_run_head)
		{
			if( (mj_global[mctx.mj_g_run].task_flags & MF_FLAG_RUNNING )== 0)
			{
				mj_global[mctx.mj_g_run].task_flags |= MF_FLAG_RUNNING;
				motion_task_start_job(mctx.mj_g_run);
				break;
			}
			else if ( (mj_global[mctx.mj_g_run].task_flags & MF_FLAG_DONE )!= 0)
			{
				mctx.mj_g_run = (mctx.mj_g_run + 1)% MF_JOB_CNT;
			}
		}
	}
}




void motion_engine_tmr_step(void)
{
	int32_t active;

	if(mctx.job != NULL)
	{
		active = 0;

		if(motion_engine_step_axis(	&mctx.job->mb_axis_head[0],&mctx.curr_pulse_pos[0],&active) != 0)
		{
			TMR_TIRGGER_X();
		}




		if( (active == 0) || (mctx.hit_active != 0))
		{
			 mctx.job->tasks_flags |= MF_FLAG_DONE;
			 mctx.job = NULL;
		}
	}
}


void motion_engine_tmr_endpos(void)
{
	uint32_t inputs;
	uint32_t is_hit   = 0;
	uint32_t hit_mask = 0;
	uint32_t ii;


	// Check if motion engine active at all
	if(mctx.job == NULL)
	{
		mctx.endpos_hit_cntr = 0;
		return;
	}

	inputs = srv_gpio_get_io();


	for(ii =0; ii < 3;ii++)
	{
		if(mctx.active_dir[ii] != 0)
		{
			if(mctx.active_dir[ii] > 0)
			{
				if( (inputs & ppctx_nv->axis[ii].endpos_max_mask) != 0)
				{
					is_hit = 1;
					hit_mask |= ppctx_nv->axis[ii].endpos_max_mask;
				}
			}
			else
			{
				if( (inputs & ppctx_nv->axis[ii].endpos_min_mask) != 0)
				{
					is_hit = 1;
					hit_mask |= ppctx_nv->axis[ii].endpos_min_mask;
				}
			}
		}
	}


	if( (inputs & ppctx_nv->estop_mask )!= 0)
	{
		is_hit = 1;
		hit_mask |= ppctx_nv->estop_mask;
	}

	if(is_hit != 0)
	{
		if(mctx.endpos_hit_cntr < HIT_LIMIT)
		{
			mctx.endpos_hit_cntr++;
		}
		else
		{
			mctx.hit_active = 1;
			mctx.hit_mask   = hit_mask;
		}
	}
	else
	{
		if(mctx.endpos_hit_cntr > 0)
		{
			mctx.endpos_hit_cntr--;
		}
	}
}


