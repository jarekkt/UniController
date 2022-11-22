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

#define  HIT_ESTOP			0x01
#define  HIT_REQSTOP		0x02

#define  HIT_ENDPOS			0x04
#define  HIT_SOFTPOS		0x08
#define  HIT_STOP			0x10
#define  HIT_RUN			0x20


#define  HOME_RUNTO_FAST	1
#define  HOME_RETRACT		2
#define  HOME_RUNFROM		3
#define  HOME_RUNUNTIL		4
#define  HOME_RUNTO_SLOW    5



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
	srv_timer_pulse_period_for_hz(mctx_nv.step_freq);

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
	xTaskCreate( motion_task, "Motion", 12 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 4, NULL );
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

void 	motion_engine_set_pos(
			int32_t	 axis_idx,
			float pos_mm
)
{
	mctx.plan_pos_mm[axis_idx] 	  = pos_mm;
	mctx.curr_pos[axis_idx]    	  = pos_mm;
	mctx.curr_pulse_pos[axis_idx] = motion_engine_units_to_pulse(pos_mm,axis_idx);


}




int32_t motion_engine_run_home_schedule(
		float						dist,
		const path_params_t 	*  	path,
		motion_home_args_t		*	home_args,
		motion_io_args_t		*	io_args,
		const burst_rcv_ctx_t   *   rcv_ctx
)
{
	motion_job_t * 	mj;
	int32_t			result = -1;

	if(motion_engine_job_init(&mj,rcv_ctx) == 0)
	{
		mj->homing = *home_args;

		if(path != NULL)
		{
			mj->jcmd = JCMD_MOTION;

			result = motion_engine_run(mj,home_args->home_axis,dist,1<<home_args->home_axis,path->speed_mm_s,path->accel_mm_s2,path->jerk_mm_s3);
			mj->io     = *io_args;
		}
		else
		{
			mj->jcmd = JCMD_MOTION_HOME_ACK;
			result = 0;
		}
	}


	return result;
}


int32_t motion_engine_run_home
(
	uint32_t 				 axis_idx,
	const burst_rcv_ctx_t  * rcv_ctx
)
{

	path_params_t			path_home;
	path_params_t			path_retract;
	float					dist_home;
	float					dist_retract;
	int32_t					result = 0;

	motion_home_args_t		home_args;
	motion_io_args_t		io_args;



	if(axis_idx >AXIS_GLOBAL_CNT)
	{
		fw_assert(0);
		return -1;
	}

	if(ppctx_nv->axis[axis_idx].homing_type == P_HOMING_NONE)
	{
		motion_job_t * 	mj;

		// Dummy homing
		motion_engine_set_pos(axis_idx,ppctx_nv->axis[axis_idx].homing_value);
		// Just send acknowledge
		if(motion_engine_job_init(&mj,rcv_ctx) == 0)
		{
			mj->jcmd 		 = JCMD_OK;
		}
		else
		{
			result = -1;
		}
	}
	else
	{
		// Real motion needed
		path_retract.speed_mm_s      = ppctx_nv->axis[axis_idx].speed_home_retract_mm_s;
		path_retract.accel_mm_s2 	 = ppctx_nv->axis[axis_idx].accel_mm_s2;
		path_retract.jerk_mm_s3      = ppctx_nv->axis[axis_idx].jerk_mm_s3;

		path_home.speed_mm_s	     = ppctx_nv->axis[axis_idx].speed_home_mm_s;
		path_home.accel_mm_s2 	     = ppctx_nv->axis[axis_idx].accel_mm_s2;
		path_home.jerk_mm_s3         = ppctx_nv->axis[axis_idx].jerk_mm_s3;


		dist_home = 1.2 *(ppctx_nv->axis[axis_idx].endpos_max_value - ppctx_nv->axis[axis_idx].endpos_min_value);
		if(dist_home == 0)
		{
			// In case soft limits not set use large unreasonable value
			dist_home = 10000;
		}

		dist_retract = ppctx_nv->axis[axis_idx].homing_retract_mm;




		home_args.home_axis				= axis_idx;
		home_args.home_value			= ppctx_nv->axis[axis_idx].homing_value;


		memset(&io_args,0,sizeof(io_args));

		switch(ppctx_nv->axis[axis_idx].homing_type)
		{
			default:
			{
				assert(0);
				result = -1;
			}break;

			case P_HOMING_TO_MIN:
			{
				io_args.io_mask_endstop	 =  ppctx_nv->axis[axis_idx].endpos_min_mask;
				io_args.io_mask_run_stop =  ppctx_nv->axis[axis_idx].homing_mask;

				if( (mctx.inputs_filtered & ppctx_nv->axis[axis_idx].homing_mask ) == 0)
				{
					// Not in end position - go there
					home_args.home_phase 	 = HOME_RUNTO_FAST;
					result 					|= motion_engine_run_home_schedule(-dist_home,&path_home,&home_args,&io_args,rcv_ctx);
				}

				if( dist_retract != 0)
				{
					// Retract
					home_args.home_phase 		= HOME_RETRACT;
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_max_mask;
					io_args.io_mask_run_stop    = 0;
					result 				   	   |= motion_engine_run_home_schedule(dist_retract,&path_home,&home_args,&io_args,rcv_ctx);

					// Home again - slowly
					home_args.home_phase 	 	= HOME_RUNTO_SLOW;
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_min_mask;
					io_args.io_mask_run_stop 	=  ppctx_nv->axis[axis_idx].homing_mask;
					result 					   |= motion_engine_run_home_schedule(-2*dist_retract,&path_retract,&home_args,&io_args,rcv_ctx);
				}
			}break;


			case P_HOMING_TO_MAX:
			{
				io_args.io_mask_endstop	 =  ppctx_nv->axis[axis_idx].endpos_max_mask;
				io_args.io_mask_run_stop =  ppctx_nv->axis[axis_idx].homing_mask;

				if( (mctx.inputs_filtered & ppctx_nv->axis[axis_idx].homing_mask ) == 0)
				{
					// Not in end position - go there
					home_args.home_phase 	 = HOME_RUNTO_FAST;
					result 					|= motion_engine_run_home_schedule(dist_home,&path_home,&home_args,&io_args,rcv_ctx);
				}

				if( dist_retract != 0)
				{
					// Retract
					home_args.home_phase 		= HOME_RETRACT;
					io_args.io_mask_endstop	 	=  ppctx_nv->axis[axis_idx].endpos_min_mask;
					io_args.io_mask_run_stop    = 0;

					result 				   	   |= motion_engine_run_home_schedule(-dist_retract,&path_home,&home_args,&io_args,rcv_ctx);

					// Home again - slowly
					home_args.home_phase 	 	= HOME_RUNTO_SLOW;
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_max_mask;
					io_args.io_mask_run_stop    =  ppctx_nv->axis[axis_idx].homing_mask;

					result 					   |= motion_engine_run_home_schedule(2*dist_retract,&path_retract,&home_args,&io_args,rcv_ctx);
				}
			}break;

			case P_HOMING_TO_MID_EDGE:
			{

				if( (mctx.inputs_filtered & ppctx_nv->axis[axis_idx].homing_mask ) != 0)
				{
					// MMID switch activated - run until inactive
					io_args.io_mask_endstop	 = ppctx_nv->axis[axis_idx].endpos_min_mask;
					home_args.home_phase 	 = HOME_RUNFROM;
					io_args.io_mask_run_stop = 0;
					io_args.io_mask_run_keep = ppctx_nv->axis[axis_idx].homing_mask;

					result 					|= motion_engine_run_home_schedule(-dist_home,&path_home,&home_args,&io_args,rcv_ctx);
				}
				else
				{
					// MMID switch not activated - run until active
					io_args.io_mask_endstop	 =  ppctx_nv->axis[axis_idx].endpos_max_mask;
					home_args.home_phase 	 = HOME_RUNTO_FAST;
					io_args.io_mask_run_stop = ppctx_nv->axis[axis_idx].homing_mask;
					io_args.io_mask_run_keep = 0;

					result 					|= motion_engine_run_home_schedule(-dist_home,&path_home,&home_args,&io_args,rcv_ctx);
				}

				if( dist_retract != 0)
				{
					// Retract
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_min_mask;
					home_args.home_phase 		= HOME_RETRACT;
					io_args.io_mask_run_keep	= 0;
					io_args.io_mask_run_keep 	= 0;

					result 				   	   |= motion_engine_run_home_schedule(-dist_retract,&path_home,&home_args,&io_args,rcv_ctx);

					// Home again - slowly
					home_args.home_phase 	 	= HOME_RUNTO_SLOW;
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_max_mask;
					io_args.io_mask_run_stop 	= ppctx_nv->axis[axis_idx].homing_mask;

					result 					   |= motion_engine_run_home_schedule(2*dist_retract,&path_retract,&home_args,&io_args,rcv_ctx);
				}
			}break;

			case P_HOMING_TO_MID_HOLE:
			{

				// Find the hole trying to go to min position
				io_args.io_mask_endstop	 = ppctx_nv->axis[axis_idx].endpos_min_mask;
				home_args.home_phase 	 = HOME_RUNUNTIL;
				io_args.io_mask_run_stop = ppctx_nv->axis[axis_idx].homing_mask | ppctx_nv->axis[axis_idx].endpos_min_mask;
				io_args.io_mask_run_keep = 0;

				result 					|= motion_engine_run_home_schedule(-dist_home,&path_home,&home_args,&io_args,rcv_ctx);

				// Find the hole trying to go to max position
				io_args.io_mask_endstop	 =  ppctx_nv->axis[axis_idx].endpos_max_mask;
				home_args.home_phase 	 =  HOME_RUNUNTIL;
				io_args.io_mask_run_stop =  ppctx_nv->axis[axis_idx].homing_mask | ppctx_nv->axis[axis_idx].endpos_max_mask;
				io_args.io_mask_run_keep =  0;

				result 					|= motion_engine_run_home_schedule(dist_home,&path_home,&home_args,&io_args,rcv_ctx);

				if( dist_retract != 0)
				{
					// Retract
					io_args.io_mask_endstop	 	= ppctx_nv->axis[axis_idx].endpos_min_mask;
					home_args.home_phase 		= HOME_RETRACT;
					io_args.io_mask_run_stop	= 0;
					io_args.io_mask_run_keep 	= 0;

					result 				   	   |= motion_engine_run_home_schedule(-dist_retract,&path_home,&home_args,&io_args,rcv_ctx);

					// Home again - slowly
					io_args.io_mask_endstop	 	=  ppctx_nv->axis[axis_idx].endpos_max_mask;
					home_args.home_phase 	 	= HOME_RUNTO_SLOW;
					io_args.io_mask_run_stop 	= ppctx_nv->axis[axis_idx].homing_mask;
					io_args.io_mask_run_keep 	= 0;

					result 					   |= motion_engine_run_home_schedule(2*dist_retract,&path_retract,&home_args,&io_args,rcv_ctx);
				}
			}break;
		}

		if(result == 0)
		{
			result 					   |= motion_engine_run_home_schedule(0,NULL,&home_args,NULL,rcv_ctx);

		}


	}



	return result;
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
		mj->mb_axis_head[axis_idx]->next = *mb_curr;
		mj->mb_axis_head[axis_idx] 		 = *mb_curr;
	}

	(*mb_curr)->next = NULL;

	return 0;
}





int32_t  motion_engine_run(motion_job_t * mj,uint32_t axis_idx,float pos_mm,uint32_t is_incremental_mask,float speed_mm_s,float accel_mm_s2,float jerk_mm_s3)
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

	if( ( is_incremental_mask &( 1<<axis_idx)) == 0)
	{
		dist_mm	 	= pos_mm - (curr_pos_mm - mctx.offset_pos_mm[axis_idx]);
	}
	else
	{
		dist_mm	 	= pos_mm;
	}



	// Update new target position
	mctx.plan_pos_mm[axis_idx] = curr_pos_mm + dist_mm;


	printd(LVL_DEBUG,"motion  engine run - axis  %d dist=%f target=%f\r\n",axis_idx,dist_mm,mctx.plan_pos_mm[axis_idx]);

	if(dist_mm	== 0)
	{
		return 0;
	}

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


		printd(LVL_TRACE,"motion  engine calc - axis %d speed=%f sspeed=%f accel=%f jerk=%f \r\n",
				axis_idx,
					calc.speed,
					calc.speed_start,
					calc.accel,
					calc.jerk
		);

		printd(LVL_TRACE,"motion  engine calc scurve - axis %d T11=%f(%f) T12=%f(%f) T13=%f(%f) T2=%f(%f) \r\n",
				axis_idx,
					calc.T11,calc.T11_s,
					calc.T12,calc.T12_s,
					calc.T13,calc.T13_s,
					calc.T2,calc.T2_s
		);

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
		mb_used = motion_engine_convert(axis_idx,curr_pos_mm,mctx.plan_pos_mm[axis_idx],step_freq,&calc,&ppctx_nv->axis[axis_idx],mb,DIM(mb));

		if(mb_used > 0)
		{
			// Inefficient way with copying - fix it later
			// TODO
			for( ii = 0; ii < mb_used;ii++)
			{
				if(motion_engine_allocate(mj,axis_idx, &mb_curr)==0)
				{

					printd(LVL_TRACE,"motion  engine calc segment - phase=%d pulse=%u\r\n",
							    axis_idx,
								mb[ii].data.mf.pulse_count
					);


					mb_curr->data = mb[ii].data;
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
	int 				result = -1;
	motion_buffer_t  *	mb_curr;


	if(motion_engine_allocate(mj,0,&mb_curr) == 0)
	{
		memset(&mb_curr->data.mf,0,sizeof(mb_curr->data.mf));

		// TODO - range check ??
		mb_curr->data.mf.tick_delay = (mctx_nv.step_freq * delay_ms) / 1000;

		result = 0;
	}


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

		case JCMD_MOTION_HOME_ACK:
		case JCMD_MOTION_WAIT:
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

				 if( (ppctx_nv->axis_used_mask &  ( 1<<idx)) != 0)
				 {
					 length += snprintf(&mctx.resp_buffer[length],
						  	  	    sizeof(mctx.resp_buffer)-length,
									" %c:%08.3f",
									axes[idx],
									mctx.curr_pos[idx] - mctx.offset_pos_mm[idx]);
				 }
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
			result = motion_engine_io(mj->jcmd_args[0],mj->jcmd_args[1]);

			if(result == 0)
			{
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"ok\r\n");
			}
			else
			{
				length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"fail\r\n");
			}
		}break;

		case JCMD_MOTION_HOME:
		{
			// These are silent
		}break;


		default:
		case JCMD_FAIL:
		{
			length = snprintf(mctx.resp_buffer, sizeof(mctx.resp_buffer),"fail\r\n");
		}break;

	}


	if(length > 0)
	{
		burst_rcv_send_response(&mj->comm_ctx,mctx.resp_buffer,length);
	}
}


static void  motion_task_abort_job(uint32_t idx)
{
	// Relese motion buffers
	mctx.mb_g_head = mj_global[idx].mb_head;

	// Report failure
	motion_engine_ack(&mj_global[idx],-1);
}


static int32_t  motion_task_finish_homing_job(uint32_t idx)
{
	int32_t result = -1;

	switch(mj_global[idx].homing.home_phase)
	{
		case HOME_RUNTO_FAST:
		{
			if( (mctx.hit_io & mj_global[idx].io.io_mask_run_stop) != 0)
			{
				result = 0;
			}
		}break;

		case HOME_RETRACT:
		{
			if( (mctx.hit_io & mj_global[idx].io.io_mask_run_stop) != 0)
			{
				result = 0;
			}
		}break;

		case HOME_RUNFROM:
		{
			if( (mctx.hit_io & mj_global[idx].io.io_mask_run_keep) == 0)
			{
				result = 0;
			}
		}break;

		case HOME_RUNUNTIL:
		{
			result = 0;
		}break;

		case HOME_RUNTO_SLOW:
		{

			if( (mctx.hit_io & mj_global[idx].io.io_mask_run_stop) != 0)
			{
				result = 0;
			}
		}break;


		default:
		{

		}break;
	}

	return result;
}

static int32_t  motion_task_finish_job(uint32_t idx)
{
	int32_t result = -1;

	if(mj_global[idx].jcmd == JCMD_MOTION)
	{
		if( (mctx.hit_active == HIT_ESTOP) || (mctx.hit_active == HIT_REQSTOP) )
		{
			// This was stop/abort request
			// Fails all remaining jobs in chain
			result = -1;
		}
		else if(mctx.hit_active != 0)
		{
			// We are executing homing - special care needed
			if(mj_global[idx].homing.home_phase != 0)
			{
				result = motion_task_finish_homing_job(idx);
			}
			else
			{
				// Ordinary failure - we hit something
				result = -1;
			}
		}
		else
		{
			// Ordinary job finish, no errors
			result = 0;
		}

		// Release motion buffers
		mctx.mb_g_tail = mj_global[idx].mb_head;
	}
	else
	{
		if(mj_global[idx].jcmd == JCMD_MOTION_HOME_ACK)
		{
			motion_task_finish_homing_job(idx);
		}

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

	if( (mj_global[idx].jcmd == JCMD_MOTION) || (mj_global[idx].jcmd == JCMD_MOTION_WAIT))
	{
		for(ii =0; ii < AXIS_GLOBAL_CNT;ii++)
		{
			// Process direction setup and changes
			if(mj_global[idx].mb_axis_head[ii] != NULL)
			{
				motion_engine_dir(ii,mj_global[idx].mb_axis_head[ii]->data.dir,mctx.active_dir);
			}
		}

		mctx.stop_active = 0;
		mctx.hit_active  = 0;
		mctx.pulse_idle  = 0;

		mctx.job 		 = &mj_global[idx];

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

	if(motion_engine_step_axis(0,&mctx.job->mb_axis_tail[0],&mctx.curr_pulse_pos[0],&mctx.curr_dir[0],mctx.active_dir,&active) != 0)
	{
		TMR_TIRGGER_X();
	}

	if(motion_engine_step_axis(1,&mctx.job->mb_axis_tail[1],&mctx.curr_pulse_pos[1],&mctx.curr_dir[1],mctx.active_dir,&active) != 0)
	{
		TMR_TIRGGER_Y();
	}


	if( ((divider ++) & 0x01) == 0 )
	{

		if(motion_engine_step_axis(3,&mctx.job->mb_axis_tail[3],&mctx.curr_pulse_pos[3],&mctx.curr_dir[3],mctx.active_dir,&active) != 0)
		{
			TMR_TIRGGER_Z();
		}


		// A/B/C/D run  half the speed

		if(motion_engine_step_axis(2,&mctx.job->mb_axis_tail[2],&mctx.curr_pulse_pos[2],&mctx.curr_dir[2],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_A();
		}

		if(motion_engine_step_axis(4,&mctx.job->mb_axis_tail[4],&mctx.curr_pulse_pos[4],&mctx.curr_dir[4],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_B();
		}

		if(motion_engine_step_axis(5,&mctx.job->mb_axis_tail[5],&mctx.curr_pulse_pos[5],&mctx.curr_dir[5],mctx.active_dir,&active) != 0)
		{
			GPIO_STEP_SET_C();
		}

		if(motion_engine_step_axis(6,&mctx.job->mb_axis_tail[6],&mctx.curr_pulse_pos[6],&mctx.curr_dir[6],mctx.active_dir,&active) != 0)
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


void motion_engine_io_filter(void)
{
	int 		ii;
	uint32_t 	mask;
	uint32_t 	new_io = 0;



	mctx.inputs = srv_gpio_get_io() ^ ppctx_nv->io_rev_mask;


	// Note that ppctx_nv->in_cpu_filter_ms is in ms, that is why there is 10x conversion

	for(ii = 0; ii < 32; ii++)
	{
		mask = (1 << ii);

		if( (mctx.inputs & mask) != 0)
		{
			if(mctx.inputs_filters[ii] < 20 * ppctx_nv->in_cpu_filter_ms)
			{
				mctx.inputs_filters[ii]++;
			}
		}
		else
		{
			if(mctx.inputs_filters[ii] > 0)
			{
				mctx.inputs_filters[ii]--;
			}
		}


		// Set the result - use hysteresis
		if( (mctx.inputs_filtered & mask) != 0)
		{
			// Previously "high"
			if( mctx.inputs_filters[ii] >  20 * ppctx_nv->in_cpu_filter_ms / 3   )
			{
				new_io |= 	mask;
			}
		}
		else
		{
			// Previously "low"
			if( mctx.inputs_filters[ii] >  40 * ppctx_nv->in_cpu_filter_ms / 3   )
			{
				new_io |= 	mask;
			}
		}
	}

	mctx.inputs_filtered = new_io;
}

int32_t motion_engine_soft_limits_active(void)
{
	int result = -1;
	int ii;


	for(ii = 0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		if(mctx.job->io.io_mask_soft_max_stop & ( 1<<ii))
		{
			if(mctx.curr_pos[ii] >= ppctx_nv->axis[ii].endpos_max_value)
			{
				result = 0;
				break;
			}
		}

		if(mctx.job->io.io_mask_soft_min_stop & ( 1<<ii))
		{
			if(mctx.curr_pos[ii] <= ppctx_nv->axis[ii].endpos_min_value)
			{
				result = 0;
				break;
			}
		}
	}

	return result;
}


void motion_engine_tmr_endpos(void)
{
	uint32_t hit_mask = 0;
	uint32_t ii;




	motion_engine_io_filter();


	for(ii =0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		mctx.curr_pos[ii] = motion_engine_pulse_to_units(mctx.curr_pulse_pos[ii],ii);
	}



	if(mctx.job == NULL)
	{
		return;
	}

	if( (mctx.job->task_flags & MF_FLAG_DONE) != 0)
	{
		return;
	}


	if( (mctx.inputs_filtered & ppctx_nv->estop_mask) != 0)
	{
		hit_mask |= HIT_ESTOP;
	}

	if( mctx.stop_active !=0 )
	{
		hit_mask |= HIT_REQSTOP;
	}

	if( (mctx.inputs_filtered & mctx.job->io.io_mask_endstop) != 0)
	{
		hit_mask |= HIT_ENDPOS;
	}

	if( (mctx.inputs_filtered & mctx.job->io.io_mask_run_stop) != 0)
	{
		hit_mask |= HIT_STOP;
	}

	if( (mctx.inputs_filtered & ~mctx.job->io.io_mask_run_keep) == 0)
	{
		hit_mask |= HIT_RUN;
	}


	if(motion_engine_soft_limits_active() == 0)
	{
		hit_mask |= HIT_SOFTPOS;
	}

	mctx.hit_active = hit_mask;
	mctx.hit_io     = mctx.inputs_filtered;

	if( (mctx.pulse_idle != 0) || (mctx.hit_active  != 0))
	{
		motion_engine_stop_timer();

		mctx.pulse_idle  	  = 0;
		mctx.job->task_flags |= MF_FLAG_DONE;

		xSemaphoreGiveFromISR(mctx.motion_kick,NULL);
	}


}


