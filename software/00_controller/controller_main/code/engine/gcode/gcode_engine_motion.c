/*!
    \file gcode_engine.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"
#include "version.h"
#include "gcode_parser.h"
#include "gcode_engine_motion.h"
#include "gcode_engine_prv.h"
#include "gcode_engine.h"

typedef struct
{
	float s_speed;
	float s_accel;
	float s_jerk;
}path_scale_t;


float gcode_engine_feedrate(float F,int32_t axis_token)
{
	float 	fr_mm_s;


	if( isnan(F) == 0)
	{
		fr_mm_s = F;
		gcd.gcx.path_param.speed_mm_s = fr_mm_s;
	}
	else
	{
		fr_mm_s = gcd.gcx.path_param.speed_mm_s;
	}

	return fr_mm_s;
}

float gcode_engine_accelerate(float G ,int32_t axis_token)
{

	float ar_mm_s2;

	if(isnan(G) == 0)
	{
		ar_mm_s2 = G;

		gcd.gcx.path_param.accel_mm_s2 = ar_mm_s2;

	}
	else
	{
		ar_mm_s2 = gcd.gcx.path_param.accel_mm_s2;
	}

	return ar_mm_s2;
}


float gcode_engine_jerk(float H,int32_t axis_token)
{

	float jr_mm_s3;

	if(isnan(H)==0)
	{
		jr_mm_s3 = H;

		gcd.gcx.path_param.jerk_mm_s3 = jr_mm_s3;
	}
	else
	{
		jr_mm_s3 = gcd.gcx.path_param.jerk_mm_s3;
	}

	return jr_mm_s3;
}


void  gcode_engine_axis_limits(int32_t idx,path_params_t * pP,path_scale_t * pScale)
{
	float   scale;

	if(pP->speed_mm_s > ppctx_nv->axis[idx].speed_mm_s)
	{
		scale    	   =  ppctx_nv->axis[idx].speed_mm_s / pP->speed_mm_s;

		if(scale < pScale->s_speed)
		{
			pScale->s_speed = scale;
		}
	}


	if(pP->accel_mm_s2 > ppctx_nv->axis[idx].accel_mm_s2)
	{
		scale 	    =  ppctx_nv->axis[idx].accel_mm_s2 / pP->accel_mm_s2;

		if(scale < pScale->s_accel)
		{
			pScale->s_accel = scale;
		}
	}


	if(pP->jerk_mm_s3 > ppctx_nv->axis[idx].jerk_mm_s3)
	{
		scale 	    =  ppctx_nv->axis[idx].jerk_mm_s3 / pP->jerk_mm_s3;

		if(scale < pScale->s_jerk)
		{
			pScale->s_jerk = scale;
		}
	}
}

void  gcode_engine_axis_scale(path_params_t * pP,path_scale_t * pScale)
{
	pP->speed_mm_s 	*= pScale->s_speed;
	pP->accel_mm_s2 *= pScale->s_accel;
	pP->jerk_mm_s3	*= pScale->s_jerk;
}


void gcode_engine_euclidean_geometry(
			motion_job_t 	    * mj,
			float			    * axis,
			uint32_t			  is_incremental,
			uint32_t			* axis_mask,
			const path_params_t	* path,
			path_params_t		* pP
)
{
	int32_t  	 ii;
	float		 vector_len = 0;
	float	     delta;
	float        scale;
	path_scale_t path_scale;

	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if( ( (*axis_mask) & ( 1<<ii))!= 0)
		{
			if( (is_incremental & (1<<ii))==0)
			{
				delta = axis[ii] - mj->pos_end_mm[ii];
			}
			else
			{
				delta = axis[ii];
			}

			if( delta != 0)
			{
				vector_len += pow(delta,2);
			}
			else
			{
				(*axis_mask) &= ~(1<<ii);
			}
		}
	}

	vector_len = sqrt(vector_len);


	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if( ( (*axis_mask) & ( 1<<ii))!= 0)
		{
			scale = axis[ii] / vector_len;
			pP[ii].speed_mm_s  = path->speed_mm_s * scale;
			pP[ii].accel_mm_s2 = path->accel_mm_s2 * scale;
			pP[ii].jerk_mm_s3  = path->jerk_mm_s3 * scale;

		}
	}

	path_scale.s_speed	= 1.0;
	path_scale.s_accel	= 1.0;
	path_scale.s_jerk 	= 1.0;


	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if( ( (*axis_mask) & ( 1<<ii))!= 0)
		{
			gcode_engine_axis_limits(ii,pP,&path_scale);
		}
	}

	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if( ( (*axis_mask) & ( 1<<ii))!= 0)
		{
			gcode_engine_axis_scale(pP,&path_scale);
		}
	}

}





int32_t gcode_engine_motion(motion_job_t * mj,float * axis, uint32_t is_incremental,float F,float G,float H)
{
	uint32_t    	axis_mask = 0;
	int32_t	    	ii;
	path_params_t	path;
	path_params_t	pP[GCODE_I_LAST_AXIS+1];


	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if( isnan(axis[ii]) == 0)axis_mask |= ( 1<< ii);
	}

	if(axis_mask == 0)
	{
		return -1;
	}

	path.speed_mm_s      = gcode_engine_feedrate(F,GCODE_I_NONE);
	path.accel_mm_s2 	 = gcode_engine_accelerate(G,GCODE_I_NONE);
	path.jerk_mm_s3      = gcode_engine_jerk(H,GCODE_I_NONE);


	// Solve geometry with Euclidean distance calculation
	gcode_engine_euclidean_geometry(mj,axis,is_incremental,&axis_mask,&path,pP);

	for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
	{
		if(axis_mask & ( 1<< ii))
		{
			motion_engine_run(mj,ii,axis[ii],is_incremental,pP[ii].speed_mm_s,pP[ii].accel_mm_s2,pP[ii].jerk_mm_s3);

			// Setup active soft limits

			if( ppctx_nv->axis[ii].endpos_min_value != ppctx_nv->axis[ii].endpos_max_value)
			{
				if(axis[ii] > 0)
				{
					mj->io.io_mask_soft_max_stop	|= (1<<ii);
				}
				else
				{
					mj->io.io_mask_endstop 			|= ppctx_nv->axis[ii].endpos_min_mask;
				}
			}

			// Setup active end position switches
			if(axis[ii] > 0)
			{
				mj->io.io_mask_endstop 			|= ppctx_nv->axis[ii].endpos_max_mask;
			}
			else
			{
				mj->io.io_mask_endstop 			|= ppctx_nv->axis[ii].endpos_min_mask;
			}
		}
	}

	// Acknowledge motion before even executing it
	motion_engine_ack(mj,JCMD_OK);

	return 0;
}


int32_t   gcode_engine_motion_G92(const burst_rcv_ctx_t * rcv_ctx,const gcode_command_t *	cmd)
{
	int32_t 		    result = 0;
	int32_t				ii;
	float 				axis[GCODE_I_LAST_AXIS+1];

	// Axis tokens
	for(ii=0;ii<=GCODE_I_LAST_AXIS;ii++)
	{
		if(cmd->tokens_present_mask & (1<< ii))
		{
			axis[ii] = gcode_engine_units(cmd->tokens[ii].value.val_float);
		}
		else
		{
			axis[ii] = NAN;
		}
	}

	motion_engine_offset_coords(axis,GCODE_I_LAST_AXIS);


	return result;
}


int32_t   gcode_engine_motion_G28(const burst_rcv_ctx_t * rcv_ctx,const gcode_command_t *	cmd)
{
	int 			ii;
	int32_t			result = -1;
	int32_t			axes_count = 0;



	// Limited support - must be only one axis specified
	for(ii=0;ii<=GCODE_I_LAST_AXIS;ii++)
	{
		if(cmd->tokens_present_mask & (1<< ii))
		{
			axes_count++;

			if(axes_count == 1)
			{
				// Axis for homing found
				result = motion_engine_run_home(ii,rcv_ctx);
			}
			else
			{
				// Only one axes allowed
				break;
			}
		}
	}

	if(result == 0)
	{
		motion_engine_jobs_start();
	}
	else
	{
		motion_engine_jobs_abort();
	}

	return result;
}




int32_t   gcode_engine_motion_G0G1(const burst_rcv_ctx_t * rcv_ctx,const gcode_command_t *	cmd,uint32_t is_incremental)
{
	float 				axis[GCODE_I_LAST_AXIS+1];
	float				F,G,H;
	int32_t				result = 0;
	motion_job_t 	  * mj;
	int32_t				ii;


	if(motion_engine_job_init(&mj,rcv_ctx) == 0)
	{
		mj->jcmd = JCMD_MOTION;

		// Axis tokens
		for(ii=0;ii<=GCODE_I_LAST_AXIS;ii++)
		{
			if(cmd->tokens_present_mask & (1<< ii))
			{
				axis[ii] = gcode_engine_units(cmd->tokens[ii].value.val_float);
			}
			else
			{
				axis[ii] = NAN;
			}
		}

		// Motion parameters tokens
		if(cmd->tokens_present_mask & (1<< GCODE_I_F))
		{
			F = gcode_engine_units(cmd->tokens[GCODE_I_F].value.val_float);
		}
		else
		{
			F = NAN;
		}

		if(cmd->tokens_present_mask & (1<< GCODE_I_G))
		{
			G = gcode_engine_units(cmd->tokens[GCODE_I_G].value.val_float);
		}
		else
		{
			G = NAN;
		}

		if(cmd->tokens_present_mask & (1<< GCODE_I_H))
		{
			H = gcode_engine_units(cmd->tokens[GCODE_I_H].value.val_float);
		}
		else
		{
			H = NAN;
		}


		result = gcode_engine_motion(mj,axis,is_incremental,F,G,H);
		if( result == 0)
		{
			motion_engine_jobs_start();
		}
		else
		{
			motion_engine_jobs_abort();
			result = -1;
		}
	}
	else
	{
		result = -1;
	}

	return result;
}

int32_t   gcode_engine_motion_G4(const burst_rcv_ctx_t * rcv_ctx,uint32_t ms_delay)
{
	motion_job_t * mj;
	int32_t		   result = 0;

	if(motion_engine_job_init(&mj,rcv_ctx) == 0)
	{
		mj->jcmd = JCMD_MOTION_WAIT;
		result 	 = motion_engine_delay(mj,ms_delay);
		if( result == 0)
		{
			motion_engine_jobs_start();
		}
		else
		{
			motion_engine_jobs_abort();
			result = -1;
		}
	}


	return result;
}


