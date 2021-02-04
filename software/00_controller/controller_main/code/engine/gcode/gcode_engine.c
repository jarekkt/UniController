/*!
    \file gcode_engine.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "gcode_parser.h"


typedef struct
{
	int32_t		  is_inch;

}gcode_ctx_t;


typedef struct
{
	gcode_ctx_t  gcx;
	char		 resp_buffer[128];
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

}


int32_t gcode_engine_units(int32_t base_unit)
{
	if(gcd.gcx.is_inch !=0)
	{
		return (base_unit * 254)/10;
	}
	else
	{
		return base_unit;
	}
}

int32_t gcode_engine_feedrate(gcode_command_t * cmd,gcode_item_e axis_token,int32_t is_home)
{
	float 	fr;
	int32_t fr_001mm_s;

	if(cmd->tokens_present_mask & (1<<GCODE_I_F))
	{
		fr = cmd->tokens[GCODE_I_F].value.val_float;
		fr_001mm_s = gcode_engine_units(fr * 1000);
	}
	else
	{
		if(is_home!=0)
		{
			fr_001mm_s = ppctx_nv->axis[axis_token].speed_001mm_s;
		}
		else
		{
			fr_001mm_s = ppctx_nv->axis[axis_token].speed_home_001mm_s;
		}
	}

	return fr_001mm_s;
}

int32_t gcode_engine_accelerate(gcode_command_t * cmd,gcode_item_e axis_token,int32_t is_home)
{
	float 	ar;
	int32_t ar_001mm_s2;

	if(cmd->tokens_present_mask & (1<<GCODE_I_G))
	{
		ar = cmd->tokens[GCODE_I_G].value.val_float;
		ar_001mm_s2 = gcode_engine_units(ar * 1000);
	}
	else
	{
		ar_001mm_s2 = ppctx_nv->axis[axis_token].accel_001mm_s2;
	}

	return ar_001mm_s2;
}


int32_t gcode_engine_jerk(gcode_command_t * cmd,gcode_item_e axis_token,int32_t is_home)
{
	float 	jr;
	int32_t jr_001mm_s3;

	if(cmd->tokens_present_mask & (1<<GCODE_I_H))
	{
		jr = cmd->tokens[GCODE_I_H].value.val_float;
		jr_001mm_s3 = gcode_engine_units(jr * 1000);
	}
	else
	{
		jr_001mm_s3 = ppctx_nv->axis[axis_token].jerk_001mm_s3;
	}

	return jr_001mm_s3;
}


int32_t gcode_engine_pos(gcode_command_t * cmd,gcode_item_e axis_token)
{
	float 	pos;
	int32_t pos_001mm_s;

	if(cmd->tokens_present_mask & (1<<axis_token))
	{
		pos = cmd->tokens[axis_token].value.val_float;
		pos_001mm_s = gcode_engine_units(pos * 1000);
	}
	else
	{
		pos_001mm_s = 0;
	}

	return pos_001mm_s;
}

void gcode_engine_command(char * cmd_line, const burst_rcv_ctx_t * rcv_ctx)
{
	int32_t 			result;
	gcode_command_t 	cmd;
	int32_t 			feedrate_001mm_s;
	int32_t 			accelerate_001mm_s2;
	int32_t 			jerk_001mm_s3;
	int32_t 			pos_001mm;
	int32_t				ii;
	uint32_t			any;

	result = gcode_parser_execute(&cmd,cmd_line);

	if(result != 0)
	{
		result = -1;
	}
	else
	{
		switch(cmd.fn)
		{
			case GCODE_F_G0:
			case GCODE_F_G1:  // linear move
			{
				for(ii = GCODE_I_X; ii <= GCODE_I_Z;ii++)
				{
					if(cmd.tokens_present_mask & (1<< ii))
					{
						feedrate_001mm_s    = gcode_engine_feedrate(&cmd,ii,0);
						accelerate_001mm_s2 = gcode_engine_accelerate(&cmd,ii,0);
						jerk_001mm_s3       = gcode_engine_jerk(&cmd,ii,0);

						pos_001mm   	    = gcode_engine_pos(&cmd,ii);

						motion_engine_run(ii,pos_001mm,feedrate_001mm_s,accelerate_001mm_s2,jerk_001mm_s3);
					}

				}
			}break;

			case GCODE_F_G20: // units inch
			{
				gcd.gcx.is_inch = 1;
			}break;

			case GCODE_F_G21: // units mm
			{
				gcd.gcx.is_inch = 0;
			}break;

			case GCODE_F_G28: // home
			{
			    any = cmd.tokens_present_mask & ( (1<<GCODE_I_X) | (1<<GCODE_I_Y) | (1<<GCODE_I_Z));

				for(ii = GCODE_I_X; ii <= GCODE_I_Z;ii++)
				{
					if( (cmd.tokens_present_mask & (1<< ii)) || (any ==0))
					{
						feedrate_001mm_s     = gcode_engine_feedrate(&cmd,ii,0);
						accelerate_001mm_s2  = gcode_engine_accelerate(&cmd,ii,0);
						jerk_001mm_s3        = gcode_engine_jerk(&cmd,ii,0);

						motion_engine_run_home(ii,feedrate_001mm_s,accelerate_001mm_s2,jerk_001mm_s3);
					}
				}
			}break;

			case GCODE_F_M101: // special
			{

			}break;

			default:
			{

			}break;
		}
	}

	if(result < 0)
	{
		burst_rcv_send_response(rcv_ctx,"FAIL\r\n",-1);
	}
	else if (result == 0)
	{
		burst_rcv_send_response(rcv_ctx,"OK\r\n",-1);
	}
	else
	{
		burst_rcv_send_response(rcv_ctx,gcd.resp_buffer,-1);
	}

}



