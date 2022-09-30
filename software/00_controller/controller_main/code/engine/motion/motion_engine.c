/*!
    \file motion_engine.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"
#include "version.h"

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

void motion_engine_start_timer(void)
{
	// Pulse length calculation - step frequency to 0.1us step pulse units
	srv_timer_pulse_period( (10*1000000 / mctx_nv.step_freq)/2);

	// Step calculations generation
	srv_timer_callback_step(mctx_nv.step_freq,motion_engine_tmr_step);
}

void motion_engine_stop_timer(void)
{
	srv_timer_callback_step_disable();
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
		for(ii =0; ii< AXIS_GLOBAL_CNT;ii++)
		{
			(*mj)->pos_beg_mm[ii] = (*mj)->pos_end_mm[ii] = mctx.plan_pos_mm[ii];
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
		for(ii =0; ii< AXIS_GLOBAL_CNT;ii++)
		{
			mctx.plan_pos_mm[ii]  =  mj_global[mj_g_run_head].pos_beg_mm[ii];
		}
	}

	// Restore buffers used
	while(mj_g_run_head != mctx.mj_g_head)
	{
		if( mj_global[mj_g_run_head].mb_head != mj_global[mj_g_run_head].mb_tail)
		{
			mctx.mb_g_head = mj_global[mj_g_run_head].mb_head;
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
	float 			speed_mm_s,
	float 			accel_mm_s2,
	float	 		jerk_mm_s3
)
{
	float dist;

	if(axis_idx >AXIS_GLOBAL_CNT)
	{
		fw_assert(0);
		return -1;
	}

	mj->args.home_axis	|= (1<<axis_idx);

	// Calculate max possible distance, add 20% to travel range
	dist = 1.2 *(ppctx_nv->axis[axis_idx].endpos_max_value - ppctx_nv->axis[axis_idx].endpos_min_value);

	if(speed_mm_s < 0)
	{
		dist 					= -dist;
		speed_mm_s				= -speed_mm_s;
		mj->args.home_axis_mask |= ppctx_nv->axis[axis_idx].endpos_min_mask;
	}
	else
	{
		mj->args.home_axis_mask |= ppctx_nv->axis[axis_idx].endpos_max_mask;
	}

	return motion_engine_run(mj,axis_idx,dist,speed_mm_s,accel_mm_s2,jerk_mm_s3);
}


void motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc)
{
	motion_buffer_t  mb[7];

	memset(&mb,0,sizeof(mb));
	motion_engine_convert(0,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[0],mb,7);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(1,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[1],mb,7);
	memset(&mb,0,sizeof(mb));
	motion_engine_convert(2,pos_001mm,0,mctx_nv.step_freq,calc,&ppctx_nv->axis[2],mb,7);
}




int32_t motion_engine_allocate(motion_job_t * mj,uint32_t axis_idx, motion_buffer_t  **	mb_curr)
{
	uint32_t       		new_mb_g_head;

	// Allocate new buffer
	new_mb_g_head = (mctx.mb_g_head + 1)% MF_BUFFER_CNT;

	if(new_mb_g_head != mctx.mb_g_tail)
	{
		*mb_curr = &mb_global[mctx.mb_g_head];

		mj->mb_head    = new_mb_g_head;
		mctx.mb_g_head = new_mb_g_head;
	}
	else
	{
		// Failure allocations
		return -1;
	}

	// Attach motion to buffers
	if(mj->mb_axis_head[axis_idx] == NULL)
	{
		// Empty list
		mj->mb_axis_head[axis_idx] = *mb_curr;
		mj->mb_axis_tail[axis_idx] = *mb_curr;
	}
	else
	{
		// Attach new motion to the end of link list
		mj->mb_axis_tail[axis_idx]->next = *mb_curr;
		mj->mb_axis_tail[axis_idx] 		 = *mb_curr;
	}

	(*mb_curr)->next = NULL;

	return 0;
}





int32_t  motion_engine_run(motion_job_t * mj,uint32_t axis_idx,float pos_mm,float speed_mm_s,float accel_mm_s2,float jerk_mm_s3)
{
	motion_calc_t  		calc;
	float		   		dist_mm;
	float		   		curr_pos_mm;
	int32_t				mb_used;
	int32_t				ii;
	motion_buffer_t  	mb[7];
	motion_buffer_t  *	mb_curr;
	uint32_t       		org_mb_g_head;
	int32_t 			result = 0;
	uint32_t			step_freq;



	// Calculate relative move
	curr_pos_mm = mctx.plan_pos_mm[axis_idx] ;
	dist_mm	 	= pos_mm - (curr_pos_mm - mctx.offset_pos_mm[axis_idx]);


	// Update new target position
	mctx.plan_pos_mm[axis_idx] = pos_mm + mctx.offset_pos_mm[axis_idx];

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

	org_mb_g_head = mctx.mb_g_head;


	if(axis_idx < AXIS_GLOBAL_CNT)
	{
		// Local controller movement

		// Calculate 3rd order S curve
		motion_scurve_calc(&calc,dist_mm, ppctx_nv->axis[axis_idx].speed_safe_mm_s,speed_mm_s,accel_mm_s2,jerk_mm_s3);

		// Convert calculation for step engine format
		if(axis_idx < AXIS_FAST_CNT)
		{
			// Fast servo axis, hardware pulse generation
			step_freq = mctx_nv.step_freq;
		}
		else
		{
			// Standard axis, software pulse generation
			step_freq = mctx_nv.step_freq / 2;
		}
		mb_used = motion_engine_convert(axis_idx,curr_pos_mm,pos_mm,step_freq,&calc,&ppctx_nv->axis[axis_idx],mb,DIM(mb));


		if(mb_used > 0)
		{
			// Inefficient way with copying - fix it later
			// TODO
			for( ii = 0; ii < mb_used;ii++)
			{
				if(motion_engine_allocate(mj,axis_idx, &mb_curr)==0)
				{
					memcpy(mb_curr,&mb[ii],sizeof(*mb_curr));
					mb_curr->next = NULL;
				}
				else
				{
					result = -1;
					break;
				}
			}
		}
	}


	if(result != 0)
	{
		// Recover allocations
		mctx.mb_g_head = org_mb_g_head;
	}

	return result;
}

int32_t motion_engine_delay
(			motion_job_t * mj,
			uint32_t	   delay_ms
)
{
	int 				result = 0;
	uint32_t   			new_mb_g_head;
	motion_buffer_t  *	mb_curr;

	mj->mb_head = mj->mb_tail = mctx.mb_g_head;
	new_mb_g_head = (mctx.mb_g_head + 1)% MF_BUFFER_CNT;

	if(new_mb_g_head != mctx.mb_g_tail)
	{
		mb_curr 	   = &mb_global[mctx.mb_g_head];
		mj->mb_head    = new_mb_g_head;
		mctx.mb_g_head = new_mb_g_head;
	}
	else
	{
		return -1;
	}

	// Empty list
	mj->mb_axis_head[0] = mb_curr;
	mj->mb_axis_tail[0] = mb_curr;
	mb_curr->next 		= NULL;

	mb_curr->dir = 0;

	memset(&mb_curr->mf,0,sizeof(mb_curr->mf));

	// TODO - range check ??
	mb_curr->mf.tick_delay = (mctx_nv.step_freq * delay_ms) / 1000;


	return result;
}



void  motion_engine_ack(motion_job_t * mj,int32_t result)
{
	uint32_t 	length = 0;
	int32_t	 	idx;
	const char  axes[]="XYZABCDE";

	switch(mj->jcmd)
	{
		case JCMD_OK:
		{
			 length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"ok\r\n");
		}break;

		case JCMD_MOTION:
		{
			 if(result == 0)
			 {
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"ok\r\n");
			 }
			 else
			 {
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"fail\r\n");
			 }
		}break;

		case JCMD_COORDS:
		{
			 length = 0;

			 length += snprintf(&mctx.resp_buffer[length],sizeof(mctx.resp_buffer)-length,"ok C: ");

			 for(idx = 0; idx < AXIS_GLOBAL_CNT;idx++)
			 {
				 length += snprintf(&mctx.resp_buffer[length],
						  	  	    sizeof(mctx.resp_buffer)-length,
									" %c:%08.3f",
									axes[idx],
									mctx.curr_pos[idx] - mctx.offset_pos_mm[idx]);
			 }

			 length += snprintf(&mctx.resp_buffer[length],sizeof(mctx.resp_buffer)-length,"\r\n");

		}break;

		case JCMD_FWINFO:
		{
			length = snprintf(mctx.resp_buffer,
						 sizeof(mctx.resp_buffer),
						 "ok\r\n PROTOCOL_VERSION: 1.0 "
						 "FIRMWARE_NAME: UniController "
						 "FIRMWARE_VERSION: %s "
						 "FIRMWARE_URL: https://github.com/jarekkt/UniController "
						 "FIRMWARE_BUILD: %s "
						 "\r\n",
						 git_hash_short,
						 __DATE__
			);
		}break;

		case JCMD_SENSORS:
		{
			length = motion_engine_io_analog(mctx.resp_buffer, sizeof(mctx.resp_buffer));
		}break;

		case JCMD_OUTPUTS:
		{
			result = motion_engine_io(mj->args.cmd_args[0],mj->args.cmd_args[1]);

			if(result == 0)
			{
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"ok\r\n");
			}
			else
			{
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"fail\r\n");
			}
		}break;


		default:
		case JCMD_FAIL:
		{
			length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"fail\r\n");
		}break;

	}


	burst_rcv_send_response(&mj->comm_ctx,mctx.resp_buffer,length);
}


static void  motion_task_abort_job(uint32_t idx)
{
	// Relese motion buffers
	mctx.mb_g_head = mj_global[idx].mb_head;

	// Report failure for remaining jobs
	motion_engine_ack(&mj_global[idx],-1);
}

static int32_t  motion_task_finish_job(uint32_t idx)
{
	int32_t result = -1;

	if(mj_global[idx].jcmd == JCMD_MOTION)
	{
		if(mctx.hit_active != 0)
		{
		   // End position sensor hit
		   if(mj_global[idx].args.home_axis != 0)
		   {
			  // It was homing move, make sure we hit proper sensor
			  // TODO
			  if( (mj_global[idx].args.home_axis_mask & mctx.hit_mask ) == mj_global[idx].args.home_axis_mask)
			  {
				  // OK, we got what we wanted
				  result = 0;
			  }
		   }
		   else
		   {
			   // Unexpected end position sensor hit
			   result = -1;
		   }

		   mctx.hit_active = 0;
		   mctx.hit_mask   = 0;
		}
		else if(mctx.stop_active != 0)
		{
			// This was stop/abort request
			result = -1;
		}
		else
		{
			//
			result = 0;
		}

		if(mj_global[idx].args.home_axis != 0)
		{
			// Only homing moves are acknowledged
			// Ordinary moves get ack immediately after entering the queue
			motion_engine_ack(&mj_global[idx],result);
		}

		// Release motion buffers
		mctx.mb_g_tail = mj_global[idx].mb_head;


	}
	else
	{
		result = 0;
		motion_engine_ack(&mj_global[idx],result);
	}

	return result;
}

void  motion_engine_offset_coords
(
			float 		* axis,
			uint32_t	  axis_cnt
)
{
	int32_t				ii;
	float				pos_abs;

	// Axis tokens
	for(ii=0;ii<=axis_cnt;ii++)
	{
		if(axis[ii] != NAN)
		{
			pos_abs = motion_engine_pulse_to_units(mctx.curr_pulse_pos[ii],ii);

			mctx.offset_pos_mm[ii] = pos_abs - axis[ii];
		}
	}
}


static uint32_t motion_task_start_job(uint32_t idx)
{
	int32_t 	ii;
	uint32_t	again = 0;

	if(mj_global[idx].jcmd == JCMD_MOTION)
	{
		for(ii =0; ii < AXIS_GLOBAL_CNT;ii++)
		{
			// Process direction setup and changes
			if(mj_global[idx].mb_axis_head[ii] != NULL)
			{
				motion_engine_dir(ii,mj_global[idx].mb_axis_head[ii]->dir,mctx.active_dir);
			}
		}

		mctx.job 		 = &mj_global[idx];
		mctx.pulse_idle  = 0;
		motion_engine_start_timer();

	}
	else
	{
		// non-motion job, mark immediately as done - will be processed in motion_task_finish_job()
		mj_global[idx].task_flags |= MF_FLAG_DONE;
		again = 1;
	}

	return again;
}



static void motion_task(void * params)
{
	uint32_t	new_mj_g_tail;
	uint32_t	again;
	int32_t     result;


	while(1)
	{
		xSemaphoreTake(mctx.motion_kick,portMAX_DELAY);

		do
		{
			again = 0;

			// Check for control requests
			if(mctx.req_stop != 0)
			{
				// TODO - fix it as now it simply aborts
				mctx.req_stop    = 0;
				mctx.stop_active = 1;
			}

			// Check if oldest job is finished
			if( mctx.mj_g_tail != mctx.mj_g_run_head)
			{
				new_mj_g_tail = (mctx.mj_g_tail + 1)% MF_JOB_CNT;

				// Check if  task is still running
				if( ( mj_global[mctx.mj_g_tail].task_flags & MF_FLAG_DONE)!= 0)
				{
					result = motion_task_finish_job(mctx.mj_g_tail);
					mctx.mj_g_tail = new_mj_g_tail;

					if(result != 0)
					{
						// Error which causes remaining jobs to be aborted
						while( mctx.mj_g_tail != mctx.mj_g_run_head)
						{
							motion_task_abort_job(mctx.mj_g_tail);
							mctx.mj_g_tail = (mctx.mj_g_tail + 1)% MF_JOB_CNT;
						}
					}

				}
			}

			//Check if oldest job needs start
			if(mctx.mj_g_tail != mctx.mj_g_run_head)
			{
				if( (mj_global[mctx.mj_g_tail].task_flags & MF_FLAG_RUNNING )== 0)
				{
					mj_global[mctx.mj_g_tail].task_flags |= MF_FLAG_RUNNING;
					again = motion_task_start_job(mctx.mj_g_tail);
				}
			}
		}while(again != 0);
	}
}




void motion_engine_tmr_step(void)
{
	static uint32_t  divider = 0;

	int32_t active;

	active = 0;

	if(motion_engine_step_axis(0,&mctx.job->mb_axis_head[0],&mctx.curr_pulse_pos[0],&mctx.curr_dir[0],mctx.active_dir,&active) != 0)
	{
		TMR_TIRGGER_X();
	}

	if(motion_engine_step_axis(1,&mctx.job->mb_axis_head[1],&mctx.curr_pulse_pos[1],&mctx.curr_dir[1],mctx.active_dir,&active) != 0)
	{
		TMR_TIRGGER_Y();
	}

	if(motion_engine_step_axis(2,&mctx.job->mb_axis_head[2],&mctx.curr_pulse_pos[2],&mctx.curr_dir[2],mctx.active_dir,&active) != 0)
	{
		TMR_TIRGGER_Z();
	}

	if( ((divider ++) & 0x01) == 0 )
	{
		// A/B/C/D run  half the speed

		if(motion_engine_step_axis(3,&mctx.job->mb_axis_head[3],&mctx.curr_pulse_pos[3],&mctx.curr_dir[3],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_A();
		}

		if(motion_engine_step_axis(4,&mctx.job->mb_axis_head[4],&mctx.curr_pulse_pos[4],&mctx.curr_dir[4],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_B();
		}

		if(motion_engine_step_axis(5,&mctx.job->mb_axis_head[5],&mctx.curr_pulse_pos[5],&mctx.curr_dir[5],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_C();
		}

		if(motion_engine_step_axis(6,&mctx.job->mb_axis_head[6],&mctx.curr_pulse_pos[6],&mctx.curr_dir[6],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_D();
		}

		if(active == 0)
		{
			 motion_engine_stop_timer();
			 mctx.pulse_idle = 1;
		}

	}
	else
	{
		GPIO_STEP_CLR_A();
		GPIO_STEP_CLR_B();
		GPIO_STEP_CLR_C();
		GPIO_STEP_CLR_D();
	}
}


void motion_engine_tmr_endpos(void)
{

	uint32_t is_hit   = 0;
	uint32_t hit_mask = 0;
	uint32_t ii;


	mctx.inputs = srv_gpio_get_io() ^ ppctx_nv->io_rev_mask;


	// Check if motion engine active at all
	if(mctx.job == NULL)
	{
		mctx.endpos_hit_cntr = 0;
		return;
	}


	for(ii =0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		if(mctx.active_dir[ii] != 0)
		{
			if(mctx.active_dir[ii] > 0)
			{
				if( (mctx.inputs & ppctx_nv->axis[ii].endpos_max_mask) != 0)
				{
					is_hit = 1;
					hit_mask |= ppctx_nv->axis[ii].endpos_max_mask;
				}
			}
			else
			{
				if( (mctx.inputs & ppctx_nv->axis[ii].endpos_min_mask) != 0)
				{
					is_hit = 1;
					hit_mask |= ppctx_nv->axis[ii].endpos_min_mask;
				}
			}
		}
	}


	if( (mctx.inputs & ppctx_nv->estop_mask )!= 0)
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

	for(ii =0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		mctx.curr_pos[ii] = motion_engine_pulse_to_units(mctx.curr_pulse_pos[ii],ii);
	}


	if( (mctx.pulse_idle != 0)
			)// || (mctx.hit_active != 0) || (mctx.stop_active != 0) )
	{
		 mctx.pulse_idle  = 0;
		 mctx.job->task_flags |= MF_FLAG_DONE;

		 xSemaphoreGiveFromISR(mctx.motion_kick,NULL);
	}


}


