/*!
    \file motion_engine_calc.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "motion_engine.h"
#include "motion_scurve.h"



int32_t  motion_engine_units_to_pulse(float units_mm,int32_t axis)
{
	double  result;

	result =  (units_mm * ((double)ppctx_nv->axis[axis].pulses_step_m)) / 1000;

	return (int32_t)result;
}



int32_t  motion_engine_units_to_enc(float units_mm,int32_t axis)
{
	double  result;

	result =  (units_mm * ((double)ppctx_nv->axis[axis].pulses_enc_m)) / 1000;

	return (int32_t)result;
}


static int64_t  motion_engine_speed_to_fract(float speed_mm_s,int32_t axis,uint32_t step_freq)
{
	int64_t	    result;

	result 			= ((int64_t)motion_engine_units_to_pulse(speed_mm_s,axis)) << 32;
	result 			= result / ((int64_t)step_freq);

	return result << 32;

}


static int64_t  motion_engine_accel_to_fract(float accel_mm_s2,int32_t axis,uint32_t step_freq)
{

	int64_t	    result;

	result 			= ((int64_t)motion_engine_units_to_pulse(accel_mm_s2,axis)) << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)step_freq);


	return result;

}

static int64_t  motion_engine_jerk_to_fract(float jerk_mm_s3,int32_t axis,uint32_t step_freq)
{
	int64_t	    result;

	result 			= ((int64_t)motion_engine_units_to_pulse(jerk_mm_s3,axis)) << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result / ((int64_t)step_freq);


	return result;

}




int32_t motion_engine_convert(
		uint32_t 				axis_idx,
		float 					from_pos_mm,
		float 					to_pos_mm,
		uint32_t 				step_freq,
		const motion_calc_t   * calc,
		const axis_params_t   * axis,
		motion_buffer_t 	  * mbfr,
		int32_t				    mbfr_cnt
)
{
	int32_t pulse_concave[2];
	int32_t pulse_line[2];
	int32_t pulse_convex[2];
	int32_t pulse_const;

	int64_t jerk_fract;
	int64_t accel_fract;
	int64_t speed_fract;
	int64_t speed_start_fract;
	int64_t speed_concave_lin_fract;
	int64_t speed_lin_convex_fract;

	int32_t mb_used = 0;
	int32_t pulse_from;
	int32_t pulse_to;
	int32_t pulse_budget;





	speed_start_fract       = motion_engine_speed_to_fract(calc->speed_start,axis_idx,step_freq);
	speed_fract				= motion_engine_speed_to_fract(calc->speed,axis_idx,step_freq);
	speed_concave_lin_fract	= motion_engine_speed_to_fract(calc->T11_v,axis_idx,step_freq);
	speed_lin_convex_fract	= motion_engine_speed_to_fract(calc->T12_v,axis_idx,step_freq);

	accel_fract     		= motion_engine_accel_to_fract(calc->accel,axis_idx,step_freq);

	jerk_fract				= motion_engine_jerk_to_fract(calc->jerk,axis_idx,step_freq);

	pulse_concave[0] 		= motion_engine_units_to_pulse(calc->T11_s,axis_idx);
	pulse_concave[1]		= pulse_concave[0];

	pulse_line[0] 			= motion_engine_units_to_pulse(calc->T12_s,axis_idx);
	pulse_line[1]			= pulse_line[0];

	pulse_convex[0] 		= motion_engine_units_to_pulse(calc->T13_s,axis_idx);
	pulse_convex[1]			= pulse_convex[0];

	pulse_const				= motion_engine_units_to_pulse(calc->T2_s,axis_idx);

	pulse_from 				= motion_engine_units_to_pulse(from_pos_mm,axis_idx);
	pulse_to 				= motion_engine_units_to_pulse(to_pos_mm,axis_idx);
	pulse_budget   			= (int32_t)fabsf(pulse_to - pulse_from);


	pulse_budget 		    -=  (pulse_concave[0] + pulse_line[0] + pulse_convex[0]);
	pulse_budget 		    -=  pulse_const;
	pulse_budget 		    -=  (pulse_concave[1] + pulse_line[1] + pulse_convex[1]);



	// Adjust rounding errors
	// These should be small values...
	if(pulse_budget > 0)
	{
		if(pulse_const > 0)
		{
			pulse_const += pulse_budget;
		}
		else
		{
			pulse_convex[0] += pulse_budget/2;
			pulse_convex[1] += pulse_budget - pulse_budget/2;
		}
	}
	else if(pulse_budget < 0)
	{
		if( pulse_const > 0)
		{
			if(pulse_const + pulse_budget > 0)
			{
				pulse_const += pulse_budget;
			}
			else
			{
				pulse_const = 0;
				pulse_budget += pulse_const;
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_concave[1] > 0)
			{
				pulse_concave[1] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_concave[1] = 0;
				pulse_budget 	+= pulse_concave[1];
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_line[1] > 0)
			{
				pulse_line[1] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_line[1] = 0;
				pulse_budget 	+= pulse_line[1];
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_convex[1] > 0)
			{
				pulse_convex[1] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_convex[1] = 0;
				pulse_budget 	+= pulse_convex[1];
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_convex[0] > 0)
			{
				pulse_convex[0] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_convex[0] = 0;
				pulse_budget 	+= pulse_convex[0];
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_line[0] > 0)
			{
				pulse_line[0] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_line[0] = 0;
				pulse_budget 	+= pulse_line[0];
			}
		}

		if(pulse_budget > 0)
		{
			if(pulse_budget + pulse_concave[0] > 0)
			{
				pulse_concave[0] += pulse_budget;
				pulse_budget     = 0;
			}
			else
			{
				pulse_concave[0] = 0;
				pulse_budget 	+= pulse_concave[0];
			}
		}
	}



	if(pulse_concave[0] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}


		mbfr[mb_used].mf.pulse_count_total   = pulse_concave[0];
		mbfr[mb_used].mf.pulse_count 		 = pulse_concave[0];
		mbfr[mb_used].mf.speed_fract 		 = speed_start_fract;
		mbfr[mb_used].mf.accel_fract		 = 0;
		mbfr[mb_used].mf.jerk_fract  		 = jerk_fract;
		mbfr[mb_used].mf.tick_delay  		 = 0;
		mbfr[mb_used].mf.accu 				 = 0;
		mbfr[mb_used].dir                    = calc->dir;
		mb_used++;
	}

	if(pulse_line[0] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}


		mbfr[mb_used].mf.pulse_count_total   = pulse_line[0];
		mbfr[mb_used].mf.pulse_count 		 = pulse_line[0];
		mbfr[mb_used].mf.speed_fract 		 = speed_concave_lin_fract;
		mbfr[mb_used].mf.accel_fract		 = accel_fract;
		mbfr[mb_used].mf.jerk_fract  		 = 0;
		mbfr[mb_used].mf.tick_delay  	     = 0;
		mbfr[mb_used].mf.accu 				 = 0;
		mbfr[mb_used].dir                    = calc->dir;
		mb_used++;
	}

	if(pulse_convex[0] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}


		mbfr[mb_used].mf.pulse_count_total   = pulse_convex[0];
		mbfr[mb_used].mf.pulse_count 		 = pulse_convex[0];
		mbfr[mb_used].mf.speed_fract 		 = speed_lin_convex_fract;
		mbfr[mb_used].mf.accel_fract		 = accel_fract;
		mbfr[mb_used].mf.jerk_fract  		 = -jerk_fract;
		mbfr[mb_used].mf.tick_delay  	     = 0;
		mbfr[mb_used].mf.accu 				 = 0;
		mbfr[mb_used].dir                    = calc->dir;
		mb_used++;
	}


	if(pulse_const > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}

		mbfr[mb_used].mf.pulse_count_total 	= pulse_const;
		mbfr[mb_used].mf.pulse_count 	   	= pulse_const;
		mbfr[mb_used].mf.speed_fract 	  	= speed_fract;
		mbfr[mb_used].mf.accel_fract		= 0;
		mbfr[mb_used].mf.jerk_fract  	  	= 0;
		mbfr[mb_used].mf.tick_delay  	    = 0;
		mbfr[mb_used].mf.accu 				 = 0;
		mbfr[mb_used].dir                   = calc->dir;
		mb_used++;
	}

	if(pulse_convex[1] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}

		mbfr[mb_used].mf.pulse_count_total  = pulse_convex[1];
		mbfr[mb_used].mf.pulse_count 		= pulse_convex[1];
		mbfr[mb_used].mf.speed_fract 		= speed_fract;
		mbfr[mb_used].mf.accel_fract		= 0;
		mbfr[mb_used].mf.jerk_fract  		= -jerk_fract;
		mbfr[mb_used].mf.tick_delay  	    = 0;
		mbfr[mb_used].mf.accu 				 = 0;
		mbfr[mb_used].dir                   = calc->dir;
		mb_used++;
	}

	if(pulse_line[1] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}

		mbfr[mb_used].mf.pulse_count_total   	= pulse_line[1];
		mbfr[mb_used].mf.pulse_count 		 	= pulse_line[1];
		mbfr[mb_used].mf.speed_fract 		 	= speed_lin_convex_fract;
		mbfr[mb_used].mf.accel_fract		 	= -accel_fract;
		mbfr[mb_used].mf.jerk_fract  		 	= 0;
		mbfr[mb_used].mf.tick_delay  	     	= 0;
		mbfr[mb_used].mf.accu 				    = 0;
		mbfr[mb_used].dir                   	= calc->dir;
		mb_used++;
	}

	if(pulse_concave[1] > 0)
	{
		if(mbfr_cnt < mb_used+1)
		{
			return -1;
		}

		mbfr[mb_used].mf.pulse_count_total  	= pulse_concave[1];
		mbfr[mb_used].mf.pulse_count 		 	= pulse_concave[1];
		mbfr[mb_used].mf.speed_fract 		 	= speed_concave_lin_fract;
		mbfr[mb_used].mf.accel_fract		 	= -accel_fract;
		mbfr[mb_used].mf.jerk_fract  		 	= jerk_fract;
		mbfr[mb_used].mf.tick_delay  	    	= 0;
		mbfr[mb_used].mf.accu 					= 0;
		mbfr[mb_used].dir                   	= calc->dir;
		mb_used++;
	}

	return mb_used;

}





