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
	gcd.gcx.path_param  	=  ppctx_nv->path_initial_m;
	gcd.gcx.is_incremental 	=  ppctx_nv->axis_inc_mask;
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
		mj->jcmd 		 = cmd;
		mj->jcmd_args[0] = cmd_arg1;
		mj->jcmd_args[1] = cmd_arg2;
		motion_engine_jobs_start();
	}
}



void gcode_engine_command(char * cmd_line, uint32_t len,const burst_rcv_ctx_t * rcv_ctx)
{
	int32_t 			result = 0;
	uint32_t 			args[2];
	gcode_command_t 	cmd;
	uint32_t			cnt;
	int					ii;

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
			{
			    printds(LVL_INFO,"GCODE engine - homing(G28)\r\n");

				// homing motion
				result = gcode_engine_motion_G28(rcv_ctx,&cmd);
				if(result != 0)
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_G0:
			case GCODE_F_G1:
			{
				printds(LVL_INFO,"GCODE engine - move(G0/G1)\r\n");

				// linear move/ homing shared code
				result = gcode_engine_motion_G0G1(rcv_ctx,&cmd,gcd.gcx.is_incremental);
				if(result != 0)
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_G4:
			{
				printds(LVL_INFO,"GCODE engine - delay(G4)\r\n");

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
				printds(LVL_INFO,"GCODE engine - inch(G20)\r\n");

				// units inch
				gcd.gcx.is_inch = 1;
				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;

			case GCODE_F_G21:
			{
				printds(LVL_INFO,"GCODE engine - mm(G21)\r\n");

				// units mm
				gcd.gcx.is_inch = 0;
				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;



			case GCODE_F_G90:
			case GCODE_F_G91:
			{

				// G90 absolute coordinates
				// G91 incremental coordinates
				cnt = 0;
				for(ii=0;ii<=GCODE_I_LAST_AXIS;ii++)
				{
					if(cmd.tokens_present_mask & (1<< ii))
					{
						cnt++;
						if(cmd.fn == GCODE_F_G90)
						{
							printd(LVL_INFO,"GCODE engine - %d axis non-incremental (G90)\r\n",ii);

							gcd.gcx.is_incremental &= ~(1<<ii);
						}
						else
						{
							printd(LVL_INFO,"GCODE engine - %d axis incremental (G90)\r\n",ii);

							gcd.gcx.is_incremental |= (1<<ii);
						}
					}
				}
				if(cnt > 0)
				{
					gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
				}
				else
				{
					gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
				}
			}break;

			case GCODE_F_G92:
			{
				printds(LVL_INFO,"GCODE engine - set position (G92)\r\n");

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
				printds(LVL_INFO,"GCODE engine - set output (M42)\r\n");

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
				printds(LVL_INFO,"GCODE engine - get sensors (M105)\r\n");

				gcode_engine_command_execute(JCMD_SENSORS,rcv_ctx,0,0);
			}break;

			case GCODE_F_M114:
			{
				printds(LVL_INFO,"GCODE engine - get coords (M114)\r\n");

				gcode_engine_command_execute(JCMD_COORDS,rcv_ctx,0,0);
			}break;

			case GCODE_F_M115:
			{
				printds(LVL_INFO,"GCODE engine - get firmware info (M115)\r\n");

				gcode_engine_command_execute(JCMD_FWINFO,rcv_ctx,0,0);

			}break;

			case GCODE_F_M204:
			{
				printds(LVL_INFO,"GCODE engine - set acceleration (M204)\r\n");

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
				printds(LVL_INFO,"GCODE engine - set jerk (M201.3)\r\n");

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
				printds(LVL_INFO,"GCODE engine - sync (M400)\r\n");

				gcode_engine_command_execute(JCMD_OK,rcv_ctx,0,0);
			}break;

			default:
			{
				gcode_engine_command_execute(JCMD_FAIL,rcv_ctx,0,0);
			}break;
		}
	}
}



