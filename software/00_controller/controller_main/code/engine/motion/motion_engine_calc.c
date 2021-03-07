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


static int64_t  motion_engine_speed_to_fract(float speed_mm_s,int32_t axis,uint32_t step_freq)
{
	int32_t		speed_001mm_s;
	int64_t	    result;

	speed_001mm_s	= (int32_t)(1000*speed_mm_s);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(speed_001mm_s,axis)) << 32;
	result 			= result / ((int64_t)step_freq);

	return result << 32;

}


static int64_t  motion_engine_accel_to_fract(float accel_mm_s2,int32_t axis,uint32_t step_freq)
{
	int32_t		accel_001mm_s2;
	int64_t	    result;

	accel_001mm_s2	= (int32_t)(1000*accel_mm_s2);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(accel_001mm_s2,axis)) << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)step_freq);


	return result;

}

static int64_t  motion_engine_jerk_to_fract(float jerk_mm_s3,int32_t axis,uint32_t step_freq)
{
	int32_t		jerk_001mm_s3;
	int64_t	    result;

	jerk_001mm_s3	= (int32_t)(1000*jerk_mm_s3);


	result 			= ((int64_t)motion_engine_pos001mm_to_pulse(jerk_001mm_s3,axis)) << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result << 32;
	result 			= result / ((int64_t)step_freq);
	result 			= result / ((int64_t)step_freq);


	return result;

}




void motion_engine_convert(
		uint32_t axis_idx,
		int32_t dist_001mm,
		int32_t pos_001mm,
		uint32_t step_freq,
		const motion_calc_t * calc,
		const axis_params_t * axis,
		motion_buffer_t 	* mbfr
)
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


	mbfr->accu  								 = 0;
	mbfr->phase 								 = MF_START_CONCAVE;
	mbfr->dir   								 = calc->dir;

#if 0
	mbfr->pos_next.pos_001mm    				 = pos_001mm + dist_001mm;
	mbfr->pos_next.pulse_count  				 = motion_engine_pos001mm_to_pulse(mbfr->pos_next.pos_001mm,axis_idx);
	mbfr->pos_next.enc_count    				 = motion_engine_pos001mm_to_enc(mbfr->pos_next.pos_001mm,axis_idx);
#endif

	speed_start_fract           				 = motion_engine_speed_to_fract(calc->speed_start,axis_idx,step_freq);
	speed_fract									 = motion_engine_speed_to_fract(calc->speed,axis_idx,step_freq);
	speed_concave_lin_fract						 = motion_engine_speed_to_fract(calc->T11_v,axis_idx,step_freq);
	speed_lin_convex_fract						 = motion_engine_speed_to_fract(calc->T12_v,axis_idx,step_freq);

	accel_fract     							 = motion_engine_accel_to_fract(calc->accel,axis_idx,step_freq);

	jerk_fract									 = motion_engine_jerk_to_fract(calc->jerk,axis_idx,step_freq);

	pulse_concave								 = motion_engine_posmm_to_pulse(calc->T11_s,axis_idx);
	pulse_line									 = motion_engine_posmm_to_pulse(calc->T12_s,axis_idx);
	pulse_convex 								 = motion_engine_posmm_to_pulse(calc->T13_s,axis_idx);

	pulse_const									 = motion_engine_posmm_to_pulse(calc->T2_s,axis_idx);

	mbfr->mf[MF_START_CONCAVE].pulse_count_total = pulse_concave;
    mbfr->mf[MF_START_CONCAVE].pulse_count 		 = pulse_concave;
    mbfr->mf[MF_START_CONCAVE].speed_fract 		 = speed_start_fract;
    mbfr->mf[MF_START_CONCAVE].accel_fract		 = 0;
    mbfr->mf[MF_START_CONCAVE].jerk_fract  		 = jerk_fract;

	mbfr->mf[MF_START_LINEAR].pulse_count_total  = pulse_line;
    mbfr->mf[MF_START_LINEAR].pulse_count 		 = pulse_line;
    mbfr->mf[MF_START_LINEAR].speed_fract 		 = speed_concave_lin_fract;
    mbfr->mf[MF_START_LINEAR].accel_fract		 = accel_fract;
    mbfr->mf[MF_START_LINEAR].jerk_fract  		 = 0;

    mbfr->mf[MF_START_CONVEX].pulse_count_total  = pulse_convex;
    mbfr->mf[MF_START_CONVEX].pulse_count 		 = pulse_convex;
    mbfr->mf[MF_START_CONVEX].speed_fract 		 = speed_lin_convex_fract;
    mbfr->mf[MF_START_CONVEX].accel_fract		 = accel_fract;
    mbfr->mf[MF_START_CONVEX].jerk_fract  		 = -jerk_fract;

    mbfr->mf[MF_CONSTANT_SPEED].pulse_count_total = pulse_const;
    mbfr->mf[MF_CONSTANT_SPEED].pulse_count 	 = pulse_const;
    mbfr->mf[MF_CONSTANT_SPEED].speed_fract 	 = speed_fract;
    mbfr->mf[MF_CONSTANT_SPEED].accel_fract		 = 0;
    mbfr->mf[MF_CONSTANT_SPEED].jerk_fract  	 = 0;

	mbfr->mf[MF_STOP_CONVEX].pulse_count_total   = pulse_convex;
    mbfr->mf[MF_STOP_CONVEX].pulse_count 		 = pulse_convex;
    mbfr->mf[MF_STOP_CONVEX].speed_fract 		 = speed_fract;
    mbfr->mf[MF_STOP_CONVEX].accel_fract		 = 0;
    mbfr->mf[MF_STOP_CONVEX].jerk_fract  		 = -jerk_fract;

	mbfr->mf[MF_STOP_LINEAR].pulse_count_total   = pulse_line;
    mbfr->mf[MF_STOP_LINEAR].pulse_count 		 = pulse_line;
    mbfr->mf[MF_STOP_LINEAR].speed_fract 		 = speed_lin_convex_fract;
    mbfr->mf[MF_STOP_LINEAR].accel_fract		 = -accel_fract;
    mbfr->mf[MF_STOP_LINEAR].jerk_fract  		 = 0;

    mbfr->mf[MF_STOP_CONCAVE].pulse_count_total  = pulse_concave;
    mbfr->mf[MF_STOP_CONCAVE].pulse_count 		 = pulse_concave;
    mbfr->mf[MF_STOP_CONCAVE].speed_fract 		 = speed_concave_lin_fract;
    mbfr->mf[MF_STOP_CONCAVE].accel_fract		 = -accel_fract;
    mbfr->mf[MF_STOP_CONCAVE].jerk_fract  		 = jerk_fract;



}





