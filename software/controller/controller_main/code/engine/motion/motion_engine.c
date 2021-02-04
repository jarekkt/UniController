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

#define  HIT_LIMIT		5


typedef struct
{
	uint32_t 			mode;

	volatile uint32_t	mt_active;
	volatile uint32_t	irq_mask;


	uint32_t			home_axis;

	uint32_t			abort_mask;
	uint32_t			stop_mask;
	uint32_t			start_mask;

	uint32_t			endpos_hit_cntr;

	int32_t				encoders[2];



	motion_timer_t		mt[AXIS_CNT];
}motion_ctx_t;

typedef struct
{
	uint32_t 			step_freq;

}motion_nv_data_t;


motion_ctx_t 	  mctx;
motion_nv_data_t  mctx_nv VAR_NV_ATTR;


static void motion_engine_tmr_step(void);
static void motion_engine_dir(int32_t idx,int32_t dir);
static void motion_engine_tmr_endpos(void);


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
}

void motion_engine_stop(uint32_t axis_mask,uint32_t abort_mask)
{

	mctx.stop_mask  |= axis_mask;
	mctx.abort_mask |= abort_mask;

	mctx.irq_mask = 1;

	while(mctx.irq_mask != 0)
	{
		//TODO - add reasonable timeout/task switch
	}

}

void motion_engine_start(uint32_t axis_mask)
{

	if(mctx.mt_active == 0)
	{
		motion_engine_reconfig();
	}


	mctx.start_mask  |= axis_mask;
	mctx.irq_mask     = 1;

	while(mctx.irq_mask != 0)
	{
		//TODO - add reasonable timeout/task switch
	}
}


void motion_engine_run_home(uint32_t axis_idx,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3)
{
	int32_t dist;

	if(axis_idx >AXIS_CNT)
	{
		fw_assert(0);
		return;
	}

	mctx.home_axis |= (1<<axis_idx);

	// Calculate max possible distance
	dist = (ppctx_nv->axis[axis_idx].endpos_max_value - ppctx_nv->axis[axis_idx].endpos_min_value) + MM_TO_001(10.0);

	if(speed_001mm_s > 0)
	{
		dist = - dist;
	}


	motion_engine_run(axis_idx,dist,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);

}


static int32_t  motion_engine_pos001mm_to_pulse(int32_t pos_001mm,int32_t axis)
{
	int64_t result;


	result =  ((int64_t)pos_001mm) * ((int64_t)ppctx_nv->axis[axis].pulses_step_100mm);
	result = result / (100*1000);

	return (int32_t)result;
}

static int32_t  motion_engine_posmm_to_pulse(float posmm,int32_t axis)
{
	int64_t result;


	result =  ((int64_t)(posmm*1000)) * ((int64_t)ppctx_nv->axis[axis].pulses_step_100mm);
	result = result / (100*1000);

	return (int32_t)result;
}



static int32_t  motion_engine_pos001mm_to_enc(int32_t pos_001mm,int32_t axis)
{
	int64_t result;

	result =  ((int64_t)pos_001mm) * ((int64_t)ppctx_nv->axis[axis].pulses_enc_100mm);
	result =   result / (100*1000);

	return (int32_t)result;
}


static int64_t  motion_engine_speed_to_fract(float speed_mm_s,int32_t axis)
{
	int32_t		speed_001mm_s;
	int64_t	    result;

	speed_001mm_s	= (int32_t)(1000*speed_mm_s);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(speed_001mm_s,axis)) << 32;
	result 			= result / ((int64_t)mctx_nv.step_freq);

	return result << 32;

}


static int64_t  motion_engine_accel_to_fract(float accel_mm_s2,int32_t axis)
{
	int32_t		accel_001mm_s2;
	int64_t	    result;

	accel_001mm_s2	= (int32_t)(1000*accel_mm_s2);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(accel_001mm_s2,axis)) << 32;
	result 			= result / ((int64_t)mctx_nv.step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)mctx_nv.step_freq);


	return result;

}

static int64_t  motion_engine_jerk_to_fract(float jerk_mm_s3,int32_t axis)
{
	int32_t		jerk_001mm_s3;
	int64_t	    result;

	jerk_001mm_s3	= (int32_t)(1000*jerk_mm_s3);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(jerk_001mm_s3,axis)) << 32;
	result 			= result / ((int64_t)mctx_nv.step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)mctx_nv.step_freq);
	result 			= result / ((int64_t)mctx_nv.step_freq);


	return result;

}




void motion_engine_convert(uint32_t axis_idx,int32_t dist_001mm,int32_t pos_001mm,const motion_calc_t  * calc,const axis_params_t * axis,motion_timer_t * tmr)
{
	int32_t pulse_concave;
	int32_t pulse_line;
	int32_t pulse_convex;
	int32_t pulse_const;

	int64_t jerk_fract;
	int64_t accel_fract;
	int64_t speed_fract;
	int64_t speed_start_fract;
	int64_t speed_concave_lin_fract;
	int64_t speed_lin_convex_fract;


	tmr->accu  									 = 0;
	tmr->phase 									 = MF_START_CONCAVE;
	tmr->dir   									 = calc->dir;

	tmr->pos_next.pos_001mm    					 = pos_001mm + dist_001mm;
	tmr->pos_next.pulse_count  					 = motion_engine_pos001mm_to_pulse(tmr->pos_next.pos_001mm,axis_idx);
	tmr->pos_next.enc_count    					 = motion_engine_pos001mm_to_enc(tmr->pos_next.pos_001mm,axis_idx);

	speed_start_fract           				 = motion_engine_speed_to_fract(calc->speed_start,axis_idx);
	speed_fract									 = motion_engine_speed_to_fract(calc->speed,axis_idx);
	speed_concave_lin_fract						 = motion_engine_speed_to_fract(calc->T11_v,axis_idx);
	speed_lin_convex_fract						 = motion_engine_speed_to_fract(calc->T12_v,axis_idx);

	accel_fract     							 = motion_engine_accel_to_fract(calc->accel,axis_idx);

	jerk_fract									 = motion_engine_jerk_to_fract(calc->jerk,axis_idx);

	pulse_concave								 = motion_engine_posmm_to_pulse(calc->T11_s,axis_idx);
	pulse_line									 = motion_engine_posmm_to_pulse(calc->T12_s,axis_idx);
	pulse_convex 								 = motion_engine_posmm_to_pulse(calc->T13_s,axis_idx);

	pulse_const									 = motion_engine_posmm_to_pulse(calc->T2_s,axis_idx);

	tmr->mf[MF_START_CONCAVE].pulse_count_total  = pulse_concave;
    tmr->mf[MF_START_CONCAVE].pulse_count 		 = pulse_concave;
    tmr->mf[MF_START_CONCAVE].speed_fract 		 = speed_start_fract;
    tmr->mf[MF_START_CONCAVE].accel_fract		 = 0;
    tmr->mf[MF_START_CONCAVE].jerk_fract  		 = jerk_fract;

	tmr->mf[MF_START_LINEAR].pulse_count_total   = pulse_line;
    tmr->mf[MF_START_LINEAR].pulse_count 		 = pulse_line;
    tmr->mf[MF_START_LINEAR].speed_fract 		 = speed_concave_lin_fract;
    tmr->mf[MF_START_LINEAR].accel_fract		 = accel_fract;
    tmr->mf[MF_START_LINEAR].jerk_fract  		 = 0;

    tmr->mf[MF_START_CONVEX].pulse_count_total   = pulse_convex;
    tmr->mf[MF_START_CONVEX].pulse_count 		 = pulse_convex;
    tmr->mf[MF_START_CONVEX].speed_fract 		 = speed_lin_convex_fract;
    tmr->mf[MF_START_CONVEX].accel_fract		 = accel_fract;
    tmr->mf[MF_START_CONVEX].jerk_fract  		 = -jerk_fract;

    tmr->mf[MF_CONSTANT_SPEED].pulse_count_total = pulse_const;
    tmr->mf[MF_CONSTANT_SPEED].pulse_count 		 = pulse_const;
    tmr->mf[MF_CONSTANT_SPEED].speed_fract 		 = speed_fract;
    tmr->mf[MF_CONSTANT_SPEED].accel_fract		 = 0;
    tmr->mf[MF_CONSTANT_SPEED].jerk_fract  		 = 0;

	tmr->mf[MF_STOP_CONVEX].pulse_count_total    = pulse_convex;
    tmr->mf[MF_STOP_CONVEX].pulse_count 		 = pulse_convex;
    tmr->mf[MF_STOP_CONVEX].speed_fract 		 = speed_fract;
    tmr->mf[MF_STOP_CONVEX].accel_fract		 	 = 0;
    tmr->mf[MF_STOP_CONVEX].jerk_fract  		 = -jerk_fract;

	tmr->mf[MF_STOP_LINEAR].pulse_count_total    = pulse_line;
    tmr->mf[MF_STOP_LINEAR].pulse_count 		 = pulse_line;
    tmr->mf[MF_STOP_LINEAR].speed_fract 		 = speed_lin_convex_fract;
    tmr->mf[MF_STOP_LINEAR].accel_fract		     = -accel_fract;
    tmr->mf[MF_STOP_LINEAR].jerk_fract  		 = 0;

    tmr->mf[MF_STOP_CONCAVE].pulse_count_total   = pulse_concave;
    tmr->mf[MF_STOP_CONCAVE].pulse_count 		 = pulse_concave;
    tmr->mf[MF_STOP_CONCAVE].speed_fract 		 = speed_concave_lin_fract;
    tmr->mf[MF_STOP_CONCAVE].accel_fract		 = -accel_fract;
    tmr->mf[MF_STOP_CONCAVE].jerk_fract  		 = jerk_fract;



}

void motion_engine_test(int32_t pos_001mm,motion_calc_t  * calc)
{
	motion_timer_t  mt;

	memset(&mt,0,sizeof(mt));
	motion_engine_convert(0,pos_001mm,0,calc,&ppctx_nv->axis[0],&mt);
	memset(&mt,0,sizeof(mt));
	motion_engine_convert(1,pos_001mm,0,calc,&ppctx_nv->axis[1],&mt);
	memset(&mt,0,sizeof(mt));
	motion_engine_convert(1,pos_001mm,0,calc,&ppctx_nv->axis[2],&mt);
}


void motion_engine_run(uint32_t axis_idx,int32_t pos_001mm,int32_t speed_001mm_s,int32_t accel_001mm_s2,int32_t jerk_001mm_s3)
{
	motion_calc_t  calc;
	int32_t		   dist_001mm;

	// Abort the move if in progress
	if((mctx.mt_active & (1<<axis_idx)) != 0)
	{
		motion_engine_stop(0,(1<<axis_idx));
	}

	dist_001mm = mctx.mt[axis_idx].pos_cur.pos_001mm - pos_001mm;

	motion_scurve_calc(&calc,dist_001mm, ppctx_nv->axis[axis_idx].speed_safe_001mm_s,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);
	motion_engine_convert(axis_idx,dist_001mm,pos_001mm,&calc,&ppctx_nv->axis[axis_idx],&mctx.mt[axis_idx]);
	motion_engine_dir(axis_idx,calc.dir);
	motion_engine_start(1<<axis_idx);
}



uint32_t motion_engine_step_axis(motion_timer_t * mt,uint32_t mask)
{
	uint64_t prev_accu;
	uint32_t pulse = 0;

next:
	if(mt->mf[mt->phase].pulse_count > 0)
	{


		prev_accu = mt->accu;
		mt->mf[mt->phase].accel_fract += mt->mf[mt->phase].jerk_fract;
		mt->mf[mt->phase].speed_fract += mt->mf[mt->phase].accel_fract;
		mt->accu += mt->mf[mt->phase].speed_fract;

		if(prev_accu >  mt->accu)
		{
			pulse = mask;
			mt->mf[mt->phase].pulse_count--;
			mt->pos_cur.pulse_count += mt->dir;
		}
	}
	else
	{
		mt->accu = 0;

		mt->phase++;
		if(mt->phase != MF_PHASES_CNT)
		{
			goto next;
		}
		else
		{
			mctx.mt_active &= ~mask;
		}
	}

	return pulse;
}




uint32_t motion_engine_run_status(void)
{
	return mctx.mt_active;
}



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
			}
		}
		mctx.stop_mask = 0;
	}
}


static void motion_engine_dir(int32_t idx,int32_t dir)
{
		switch(idx)
		{
			case AXIS_X:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_EXT2);
				}
				else
				{
					GPIO_Clr(OUT_EXT2);
				}
			}break;

			case AXIS_Y:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_EXT4);
				}
				else
				{
					GPIO_Clr(OUT_EXT4);
				}

			}break;

			case AXIS_Z:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_EXT6);
				}
				else
				{
					GPIO_Clr(OUT_EXT6);
				}
			}break;
		}
}

static void motion_engine_tmr_step(void)
{
	if(mctx.mt_active & 0x01)
	{
		if(motion_engine_step_axis(&mctx.mt[AXIS_X],0x01) != 0)
		{
			TMR_TIRGGER_X();
		}
	}

	if(mctx.mt_active & 0x02)
	{
		if( motion_engine_step_axis(&mctx.mt[AXIS_Y],0x02)!=0)
		{
			TMR_TIRGGER_Y();
		}
	}

	if(mctx.mt_active & 0x04)
	{
		if(motion_engine_step_axis(&mctx.mt[AXIS_Z],0x04)!=0)
		{
			TMR_TIRGGER_Z();
		}
	}


	if(mctx.irq_mask != 0)
	{
		motion_engine_tmr_masks();
		mctx.irq_mask = 0;
	}

	if(mctx.mt_active == 0)
	{
		srv_timer_callback_step_disable();
	}


}

static void motion_engine_tmr_endpos(void)
{
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
			if(&mctx.mt[ii].dir ==0)
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
}


