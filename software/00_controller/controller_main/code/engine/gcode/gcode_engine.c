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
#include "gcode_engine.h"
#include "gcode_engine_motion.h"
#include "gcode_engine_prv.h"




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


void gcode_engine_command_execute(jcmd_e cmd,const burst_rcv_ctx_t * rcv_ctx,uint32_t cmd_arg1,uint32_t cmd_arg2)
{
	motion_job_t * mj;

	if(motion_engine_job_init(&mj,rcv_ctx) == 0)
	{
		mj->jcmd 		 	 = cmd;
		mj->args.cmd_args[0] = cmd_arg1;
		mj->args.cmd_args[1] = cmd_arg2;
		motion_engine_jobs_start();
	}
}



void gcode_engine_command(char * cmd_line, uint32_t len,const burst_rcv_ctx_t * rcv_ctx)
{
	int32_t 			result = 0;
	uint32_t 			args[2];
	gcode_command_t 	cmd;

	float				S;

	result = gcode_parser_execute(&cmd,cmd_line,len);

	if(result != 0)
	{
		result = -1;
	}
	else
	{
		switch(cmd.fn)
		{
			case GCODE_F_G28:
			case GCODE_F_G0:
			case GCODE_F_G1:
			{
				// linear move/ homing shared code
				result = gcode_engine_motion_G0G1(rcv_ctx,&cmd);
				if(result != 0)
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_G4:
			{
				if(  (cmd.tokens_present_mask & (1<< GCODE_I_S) ))
				{
					args[0] = 1000 * cmd.tokens[GCODE_I_S].value.val_float;
					result = gcode_engine_motion_G4(rcv_ctx,args[0]);
				}
				else
				{
					result = -1;
				}

				if(result != 0)
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_G20:
			{
				// units inch
				gcd.gcx.is_inch = 1;
				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;

			case GCODE_F_G21:
			{
				// units mm
				gcd.gcx.is_inch = 0;
				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;

			case GCODE_F_G92:
			{
				result = gcode_engine_motion_G92(rcv_ctx,&cmd);
				if(result != 0)
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
				else
				{
					gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_M42:
			{
				if(  (cmd.tokens_present_mask & (1<< GCODE_I_S)) &&
					 (cmd.tokens_present_mask & (1<< GCODE_I_P))
				)
				{
					args[0] = cmd.tokens[GCODE_I_P].value.val_ui32;
					args[1] = cmd.tokens[GCODE_I_S].value.val_ui32;

					gcode_engine_command_execute(JCMD_OUTPUTS,rcv_ctx,args[0],args[1]);
				}
				else
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}

			}break;

			case GCODE_F_M105:
			{
				gcode_engine_command_execute(JCMD_SENSORS,rcv_ctx,0,0);
			}break;

			case GCODE_F_M114:
			{
				gcode_engine_command_execute(JCMD_COORDS,rcv_ctx,0,0);
			}break;

			case GCODE_F_M115:
			{
				gcode_engine_command_execute(JCMD_FWINFO,rcv_ctx,0,0);

			}break;

			case GCODE_F_M204:
			{
				if(cmd.tokens_present_mask & (1<< GCODE_I_S))
				{
					S = cmd.tokens[GCODE_I_S].value.val_float;
					gcd.gcx.path_param.accel_mm_s2 = gcode_engine_units(S * 1000);
					gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
				}
				else
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}

			}break;

			case GCODE_F_M201_3:
			{
				if(cmd.tokens_present_mask & (1<< GCODE_I_S))
				{
					S = cmd.tokens[GCODE_I_S].value.val_float;
					gcd.gcx.path_param.jerk_mm_s3 = gcode_engine_units(S * 1000);
					gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
				}
				else
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}

			}break;

			case GCODE_F_M400:
			{
				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;

			default:
			{
				gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
			}break;
		}
	}
}



