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




typedef struct
{
	int32_t		  			is_inch;
	path_params_t		 	path_param;
}gcode_ctx_t;




typedef struct
{
	gcode_ctx_t  			 gcx;
	char					 resp_buffer[256];


}gcode_data_t;


typedef struct
{
	int32_t dummy;
}gcode_nv_data_t;




gcode_data_t     gcd;
gcode_nv_data_t  gcd_nv VAR_NV_ATTR;


void gcode_engine_init_default(void)
{
	memset(&gcd_nv,0,sizeof(gcd_nv));
	memset(&gcd,0,sizeof(gcd));
}

void gcode_engine_init(void)
{
	gcode_parser_init();

	memset(&gcd,0,sizeof(gcd));

	srv_nov_register(&gcd_nv,sizeof(gcd_nv),gcode_engine_init_default);
}


void gcode_engine_once(void)
{
	// Initial setup for speed/acceleration/jerk
	gcd.gcx.path_param  =ppctx_nv->path_initial_m;
}


float gcode_engine_units(float base_unit)
{
	if(gcd.gcx.is_inch !=0)
	{
		return (base_unit * 25.4);
	}
	else
	{
		return base_unit;
	}
}

float gcode_engine_feedrate(float F,int32_t axis_token,int32_t is_home)
{
	float 	fr_mm_s;


	if( isnan(F) == 0)
	{
		fr_mm_s = F;

		if(is_home==0)
		{
			gcd.gcx.path_param.speed_mm_s = fr_mm_s;
		}
	}
	else
	{
		if(is_home==0)
		{
			fr_mm_s = gcd.gcx.path_param.speed_mm_s;
		}
		else
		{
			if(axis_token <= GCODE_I_LAST_AXIS)
			{
				fr_mm_s = ppctx_nv->axis[axis_token].speed_home_mm_s;
			}
			else
			{
				fr_mm_s = 0;
			}
		}
	}

	return fr_mm_s;
}

float gcode_engine_accelerate(float G ,int32_t axis_token,int32_t is_home)
{

	float ar_mm_s2;

	if(isnan(G) == 0)
	{
		ar_mm_s2 = G;

		if(is_home==0)
		{
			gcd.gcx.path_param.accel_mm_s2 = ar_mm_s2;
		}
	}
	else
	{
		if(is_home==0)
		{
			ar_mm_s2 = gcd.gcx.path_param.accel_mm_s2;
		}
		else
		{
			if(axis_token <= GCODE_I_LAST_AXIS)
			{
				ar_mm_s2 = ppctx_nv->axis[axis_token].accel_mm_s2;
			}
			else
			{
				ar_mm_s2 = 0;
			}

		}
	}

	return ar_mm_s2;
}


float gcode_engine_jerk(float H,int32_t axis_token,int32_t is_home)
{

	float jr_mm_s3;

	if(isnan(H)==0)
	{
		jr_mm_s3 = H;

		if(is_home==0)
		{
			gcd.gcx.path_param.jerk_mm_s3 = jr_mm_s3;
		}
	}
	else
	{
		if(is_home==0)
		{
			jr_mm_s3 = gcd.gcx.path_param.jerk_mm_s3;
		}
		else
		{
			if(axis_token <= GCODE_I_LAST_AXIS)
			{
				jr_mm_s3 = ppctx_nv->axis[axis_token].jerk_mm_s3;
			}
			else
			{
				jr_mm_s3 = 0;
			}
		}
	}

	return jr_mm_s3;
}


int32_t gcode_engine_geometry_XYZ(
			float 			  	  X,
			float 				  Y,
			float 				  Z,
			const path_params_t	* path,
			path_params_t		* pX,
			path_params_t		* pY,
			path_params_t		* pZ
)
{

}



int32_t gcode_engine_motion(motion_job_t * mj,float * axis, float F,float G,float H,int32_t is_homing)
{
	uint32_t    	home_mask = 0;
	int32_t	    	ii;
	int32_t 		result = -1;
	path_params_t	path;
	path_params_t	pX,pY,pZ;

	if(is_homing!= 0)
	{
		// Homing move required
		for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
		{
			if( isnan(axis[ii]) != 0)home_mask |= ( 1<< ii);
		}

		for( ii = GCODE_I_X; ii <= GCODE_I_LAST_AXIS;ii++)
		{
			if(home_mask & ( 1<< ii))
			{
				path.speed_mm_s     = gcode_engine_feedrate(F,ii,1);
				path.accel_mm_s2    = gcode_engine_accelerate(G,ii,1);
				path.jerk_mm_s3     = gcode_engine_jerk(H,ii,1);

				result = motion_engine_run_home(
						mj,
						ii,
						path.speed_mm_s,
						path.accel_mm_s2,
						path.jerk_mm_s3
				);
			 }
		}
	}
	else
	{
		path.speed_mm_s      = gcode_engine_feedrate(F,GCODE_I_NONE,0);
		path.accel_mm_s2 	 = gcode_engine_accelerate(G,GCODE_I_NONE,0);
		path.jerk_mm_s3      = gcode_engine_jerk(H,GCODE_I_NONE,0);

		switch( ppctx_nv->geometry)
		{
			case 0:
			{
#if 0
				if(  (isnan(X) == 0) || (isnan(Y) == 0) )
				{
					// Interlocked X/Y/Z
					if(isnan(Z) == 0)
					{
						// Rare case, combined X/Y/Z move
						result = gcode_engine_geometry_XYZ(X,Y,Z,&path,&pX,&pY,&pZ);
					}
					else
					{
						// Pure X/Y
					}
				}
				else
				{
					// Non-interlocked moves

				}
#endif
			}break;

			default:
			{
				result = -1;
			}
		}




		//motion_engine_run(mj,ii,pos_001mm,feedrate_001mm_s,accelerate_001mm_s2,jerk_001mm_s3);
	}

	return result;
}


void gcode_engine_command(char * cmd_line, const burst_rcv_ctx_t * rcv_ctx)
{
	int32_t 			result = 0;
	gcode_command_t 	cmd;
	float 				axis[GCODE_I_LAST_AXIS+1];
	float				F,G,H;
	float				S;
	int32_t				is_homing = -1;
	motion_job_t 	  * mj;
	int32_t				ii;



	result = gcode_parser_execute(&cmd,cmd_line);

	if(result != 0)
	{
		result = -1;
	}
	else
	{
		switch(cmd.fn)
		{
			case GCODE_F_G28:
			{
				is_homing = 0;
			} // pass further to G0/G1
			case GCODE_F_G0:
			case GCODE_F_G1:  // linear move/ homing shared code
			{
				if(motion_engine_job_init(&mj,rcv_ctx) == 0)
				{
					// Axis tokens
					for(ii=0;ii<=GCODE_I_LAST_AXIS;ii++)
					{
						if(cmd.tokens_present_mask & (1<< ii))
						{
							axis[ii] = gcode_engine_units(cmd.tokens[ii].value.val_float);
						}
						else
						{
							axis[ii] = NAN;
						}
					}

					// Motion parameters tokens
					if(cmd.tokens_present_mask & (1<< GCODE_I_F))
					{
						F = gcode_engine_units(cmd.tokens[GCODE_I_F].value.val_float);
					}

					if(cmd.tokens_present_mask & (1<< GCODE_I_G))
					{
						G = gcode_engine_units(cmd.tokens[GCODE_I_G].value.val_float);
					}

					if(cmd.tokens_present_mask & (1<< GCODE_I_H))
					{
						H = gcode_engine_units(cmd.tokens[GCODE_I_H].value.val_float);
					}

					if(gcode_engine_motion(mj,axis,F,G,H,is_homing) != 0)
					{
						motion_engine_jobs_start();
					}
					else
					{
						motion_engine_jobs_abort();
					}
				}
				else
				{
					result = -1;
				}

			}break;

			case GCODE_F_G20: // units inch
			{
				gcd.gcx.is_inch = 1;
				result 			= 1;
			}break;

			case GCODE_F_G21: // units mm
			{
				gcd.gcx.is_inch = 0;
				result 			= 1;
			}break;

			case GCODE_F_M101: // special
			{

			}break;

			case GCODE_F_M201:
			{
				if(cmd.tokens_present_mask & (1<< GCODE_I_S))
				{
					S = cmd.tokens[GCODE_I_S].value.val_float;
					gcd.gcx.path_param.accel_mm_s2 = gcode_engine_units(S * 1000);
				}
			}break;

			case GCODE_F_M201_3:
			{
				if(cmd.tokens_present_mask & (1<< GCODE_I_S))
				{
					S = cmd.tokens[GCODE_I_S].value.val_float;
					gcd.gcx.path_param.jerk_mm_s3 = gcode_engine_units(S * 1000);
				}
			}break;

			case GCODE_F_M115:
			{
				result = snprintf(gcd.resp_buffer,
						 sizeof(gcd.resp_buffer),
						 "ok PROTOCOL_VERSION: 1.0 "
						 "FIRMWARE_NAME: UniController %d.%d "
						 "FIRMWARE_URL: https://github.com/jarekkt/UniController "
						 "FIRMWARE_GITHASH: %s \r\n",
						 0,1,
						 git_hash_short
				);

			}break;

			default:
			{

			}break;
		}
	}

	if(result < 0)
	{
		burst_rcv_send_response(rcv_ctx,"fail\r\n",-1);
	}
	else if (result > 0)
	{
		burst_rcv_send_response(rcv_ctx,gcd.resp_buffer,result);
	}
	else
	{
		// delayed response
	}

}



