/*!
    \file params.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "params.h"


typedef struct
{

	char		resp_value[128];
}params_ctx_t;



params_ctx_t			  pctx;
params_nv_ctx_t  		  pctx_nv VAR_NV_ATTR;



const params_nv_ctx_t  *  ppctx_nv = &pctx_nv;


const var_ptable_t   params_var_ptable[] SERMON_ATTR =
{
  { "x_pulses_step_m",   		&pctx_nv.axis[AXIS_X].pulses_step_m     	,E_VA_UINT_FREE  },
  { "x_pulses_enc_m", 		  	&pctx_nv.axis[AXIS_X].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "x_endpos_min_mask",   		&pctx_nv.axis[AXIS_X].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "x_endpos_max_mask",   		&pctx_nv.axis[AXIS_X].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "x_endpos_min_value",   	&pctx_nv.axis[AXIS_X].endpos_min_value      ,E_VA_FLT_FREE   },
  { "x_endpos_max_value",   	&pctx_nv.axis[AXIS_X].endpos_max_value      ,E_VA_FLT_FREE   },
  { "x_homing_type",   			&pctx_nv.axis[AXIS_X].homing_type 		    ,E_VA_UINT_FREE  },
  { "x_homing_mask",   			&pctx_nv.axis[AXIS_X].homing_mask      		,E_VA_UINT_FREE  },
  { "x_homing_value",  			&pctx_nv.axis[AXIS_X].homing_value      	,E_VA_FLT_FREE   },
  { "x_homing_retact_mm",  		&pctx_nv.axis[AXIS_X].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "x_speed_mm_s",   			&pctx_nv.axis[AXIS_X].speed_mm_s       		,E_VA_FLT_FREE   },
  { "x_speed_home_mm_s", 		&pctx_nv.axis[AXIS_X].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "x_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_X].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "x_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_X].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "x_accel_mm_s2",   			&pctx_nv.axis[AXIS_X].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "x_jerk_mm_s3",   			&pctx_nv.axis[AXIS_X].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "y_pulses_step_m",   		&pctx_nv.axis[AXIS_Y].pulses_step_m     	,E_VA_UINT_FREE  },
  { "y_pulses_enc_m",		   	&pctx_nv.axis[AXIS_Y].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "y_endpos_min_mask",   		&pctx_nv.axis[AXIS_Y].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_max_mask",   		&pctx_nv.axis[AXIS_Y].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_min_value",   	&pctx_nv.axis[AXIS_Y].endpos_min_value      ,E_VA_FLT_FREE   },
  { "y_endpos_max_value",   	&pctx_nv.axis[AXIS_Y].endpos_max_value      ,E_VA_FLT_FREE   },
  { "y_homing_type",   			&pctx_nv.axis[AXIS_Y].homing_type 		    ,E_VA_UINT_FREE  },
  { "y_homing_mask",   			&pctx_nv.axis[AXIS_Y].homing_mask      		,E_VA_UINT_FREE  },
  { "y_homing_value",  			&pctx_nv.axis[AXIS_Y].homing_value      	,E_VA_FLT_FREE   },
  { "y_homing_retact_mm",  		&pctx_nv.axis[AXIS_Y].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "y_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_Y].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "y_speed_mm_s", 			&pctx_nv.axis[AXIS_Y].speed_mm_s       		,E_VA_FLT_FREE   },
  { "y_speed_home_mm_s", 		&pctx_nv.axis[AXIS_Y].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "y_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_Y].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "y_accel_mm_s2",   			&pctx_nv.axis[AXIS_Y].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "y_jerk_mm_s3",   			&pctx_nv.axis[AXIS_Y].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "z_pulses_step_m",		   	&pctx_nv.axis[AXIS_Z].pulses_step_m     	,E_VA_UINT_FREE  },
  { "z_pulses_enc_m",   		&pctx_nv.axis[AXIS_Z].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "z_endpos_min_mask",   		&pctx_nv.axis[AXIS_Z].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "z_endpos_max_mask",   		&pctx_nv.axis[AXIS_Z].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "z_endpos_min_value",   	&pctx_nv.axis[AXIS_Z].endpos_min_value      ,E_VA_FLT_FREE   },
  { "z_endpos_max_value",   	&pctx_nv.axis[AXIS_Z].endpos_max_value      ,E_VA_FLT_FREE   },
  { "z_homing_type",   			&pctx_nv.axis[AXIS_Z].homing_type 		    ,E_VA_UINT_FREE  },
  { "z_homing_mask",   			&pctx_nv.axis[AXIS_Z].homing_mask      		,E_VA_UINT_FREE  },
  { "z_homing_value",  			&pctx_nv.axis[AXIS_Z].homing_value      	,E_VA_FLT_FREE   },
  { "z_homing_retact_mm",  		&pctx_nv.axis[AXIS_Z].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "z_speed_mm_s",  			&pctx_nv.axis[AXIS_Z].speed_mm_s       		,E_VA_FLT_FREE   },
  { "z_speed_home_mm_s", 		&pctx_nv.axis[AXIS_Z].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "z_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_Z].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "z_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_Z].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "z_accel_mm_s2",   			&pctx_nv.axis[AXIS_Z].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "z_jerk_mm_s3",   			&pctx_nv.axis[AXIS_Z].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "a_pulses_step_m",		   	&pctx_nv.axis[AXIS_A].pulses_step_m     	,E_VA_UINT_FREE  },
  { "a_endpos_min_mask",   		&pctx_nv.axis[AXIS_A].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_max_mask",   		&pctx_nv.axis[AXIS_A].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_min_value",   	&pctx_nv.axis[AXIS_A].endpos_min_value      ,E_VA_FLT_FREE   },
  { "a_endpos_max_value",   	&pctx_nv.axis[AXIS_A].endpos_max_value      ,E_VA_FLT_FREE   },
  { "a_homing_retact_mm",  		&pctx_nv.axis[AXIS_A].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "a_homing_type",   			&pctx_nv.axis[AXIS_A].homing_type 		    ,E_VA_UINT_FREE  },
  { "a_homing_mask",   			&pctx_nv.axis[AXIS_A].homing_mask      		,E_VA_UINT_FREE  },
  { "a_homing_value",  			&pctx_nv.axis[AXIS_A].homing_value      	,E_VA_FLT_FREE   },
  { "a_speed_mm_s",  			&pctx_nv.axis[AXIS_A].speed_mm_s       		,E_VA_FLT_FREE   },
  { "a_speed_home_mm_s", 		&pctx_nv.axis[AXIS_A].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "a_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_A].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "a_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_A].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "a_accel_mm_s2",   			&pctx_nv.axis[AXIS_A].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "a_jerk_mm_s3",   			&pctx_nv.axis[AXIS_A].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "b_pulses_step_m",   		&pctx_nv.axis[AXIS_B].pulses_step_m     	,E_VA_UINT_FREE  },
  { "b_endpos_min_mask",   		&pctx_nv.axis[AXIS_B].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_max_mask",   		&pctx_nv.axis[AXIS_B].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_min_value",   	&pctx_nv.axis[AXIS_B].endpos_min_value      ,E_VA_FLT_FREE   },
  { "b_endpos_max_value",   	&pctx_nv.axis[AXIS_B].endpos_max_value      ,E_VA_FLT_FREE   },
  { "b_homing_type",   			&pctx_nv.axis[AXIS_B].homing_type 		    ,E_VA_UINT_FREE  },
  { "b_homing_mask",   			&pctx_nv.axis[AXIS_B].homing_mask      		,E_VA_UINT_FREE  },
  { "b_homing_value",  			&pctx_nv.axis[AXIS_B].homing_value      	,E_VA_FLT_FREE   },
  { "b_homing_retact_mm",  		&pctx_nv.axis[AXIS_B].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "b_speed_mm_s",   			&pctx_nv.axis[AXIS_B].speed_mm_s       		,E_VA_FLT_FREE   },
  { "b_speed_home_mm_s", 		&pctx_nv.axis[AXIS_B].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "b_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_B].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "b_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_B].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "b_accel_mm_s2",   			&pctx_nv.axis[AXIS_B].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "b_jerk_mm_s3",   			&pctx_nv.axis[AXIS_B].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "c_pulses_step_m",   		&pctx_nv.axis[AXIS_C].pulses_step_m     	,E_VA_UINT_FREE  },
  { "c_endpos_min_mask",   		&pctx_nv.axis[AXIS_C].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "c_endpos_max_mask",   		&pctx_nv.axis[AXIS_C].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "c_endpos_min_value",   	&pctx_nv.axis[AXIS_C].endpos_min_value      ,E_VA_FLT_FREE   },
  { "c_endpos_max_value",   	&pctx_nv.axis[AXIS_C].endpos_max_value      ,E_VA_FLT_FREE   },
  { "c_homing_retact_mm",  		&pctx_nv.axis[AXIS_C].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "c_homing_type",   			&pctx_nv.axis[AXIS_C].homing_type 		    ,E_VA_UINT_FREE  },
  { "c_homing_mask",   			&pctx_nv.axis[AXIS_C].homing_mask      		,E_VA_UINT_FREE  },
  { "c_homing_value",  			&pctx_nv.axis[AXIS_C].homing_value      	,E_VA_FLT_FREE   },
  { "c_speed_mm_s", 			&pctx_nv.axis[AXIS_C].speed_mm_s       		,E_VA_FLT_FREE   },
  { "c_speed_home_mm_s", 		&pctx_nv.axis[AXIS_C].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "c_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_C].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "c_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_C].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "c_accel_mm_s2",   			&pctx_nv.axis[AXIS_C].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "c_jerk_mm_s3",   			&pctx_nv.axis[AXIS_C].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "d_pulses_step_m",		   	&pctx_nv.axis[AXIS_D].pulses_step_m     	,E_VA_UINT_FREE  },
  { "d_endpos_min_mask",   		&pctx_nv.axis[AXIS_D].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_max_mask",   		&pctx_nv.axis[AXIS_D].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_min_value",   	&pctx_nv.axis[AXIS_D].endpos_min_value      ,E_VA_FLT_FREE   },
  { "d_endpos_max_value",   	&pctx_nv.axis[AXIS_D].endpos_max_value      ,E_VA_FLT_FREE   },
  { "d_homing_type",   			&pctx_nv.axis[AXIS_D].homing_type 		    ,E_VA_UINT_FREE  },
  { "d_homing_mask",   			&pctx_nv.axis[AXIS_D].homing_mask      		,E_VA_UINT_FREE  },
  { "d_homing_value",  			&pctx_nv.axis[AXIS_D].homing_value      	,E_VA_FLT_FREE   },
  { "d_homing_retact_mm",  		&pctx_nv.axis[AXIS_D].homing_retract_mm     ,E_VA_FLT_FREE   },
  { "d_speed_mm_s",  			&pctx_nv.axis[AXIS_D].speed_mm_s       		,E_VA_FLT_FREE   },
  { "d_speed_home_mm_s", 		&pctx_nv.axis[AXIS_D].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "d_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_D].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "d_speed_home_retractmm_s",	&pctx_nv.axis[AXIS_D].speed_home_retract_mm_s,E_VA_FLT_FREE  },
  { "d_accel_mm_s2",   			&pctx_nv.axis[AXIS_D].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "d_jerk_mm_s3",   			&pctx_nv.axis[AXIS_D].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "p_speed_mm_s",  			&pctx_nv.path_initial_m.speed_mm_s      	,E_VA_FLT_FREE   },
  { "p_accel_mm_s2",   			&pctx_nv.path_initial_m.accel_mm_s2     	,E_VA_FLT_FREE   },
  { "p_jerk_mm_s3",   			&pctx_nv.path_initial_m.jerk_mm_s3      	,E_VA_FLT_FREE   },

  { "pwm_out1_min",		 	  	&pctx_nv.pwm_range[PWM_OUT1].min    		,E_VA_UINT_FREE  },
  { "pwm_out1_max", 	  		&pctx_nv.pwm_range[PWM_OUT1].max  			,E_VA_UINT_FREE  },
  { "pwm_out1_delay", 	  		&pctx_nv.pwm_range[PWM_OUT1].delay_ms 		,E_VA_UINT_FREE  },

  { "pwm_out2_min",		 	  	&pctx_nv.pwm_range[PWM_OUT2].min    		,E_VA_UINT_FREE  },
  { "pwm_out2_max", 	  		&pctx_nv.pwm_range[PWM_OUT2].max  			,E_VA_UINT_FREE  },
  { "pwm_out2_delay", 	  		&pctx_nv.pwm_range[PWM_OUT2].delay_ms 		,E_VA_UINT_FREE  },

  { "pwm_out3_min",		 	  	&pctx_nv.pwm_range[PWM_OUT3].min    		,E_VA_UINT_FREE  },
  { "pwm_out3_max", 	  		&pctx_nv.pwm_range[PWM_OUT3].max  			,E_VA_UINT_FREE  },
  { "pwm_out3_delay", 	  		&pctx_nv.pwm_range[PWM_OUT3].delay_ms 		,E_VA_UINT_FREE  },

  { "pwm_out4_min",		 	  	&pctx_nv.pwm_range[PWM_OUT4].min    		,E_VA_UINT_FREE  },
  { "pwm_out4_max", 	  		&pctx_nv.pwm_range[PWM_OUT4].max  			,E_VA_UINT_FREE  },
  { "pwm_out4_delay", 	  		&pctx_nv.pwm_range[PWM_OUT4].delay_ms 		,E_VA_UINT_FREE  },

  { "pwm_out11_min",		 	&pctx_nv.pwm_range[PWM_OUT11].min   		,E_VA_UINT_FREE  },
  { "pwm_out11_max", 	  		&pctx_nv.pwm_range[PWM_OUT11].max  			,E_VA_UINT_FREE  },
  { "pwm_out11_delay", 	  		&pctx_nv.pwm_range[PWM_OUT11].delay_ms  	,E_VA_UINT_FREE  },

  { "pwm_out12_min",		 	&pctx_nv.pwm_range[PWM_OUT12].min   		,E_VA_UINT_FREE  },
  { "pwm_out12_max", 	  		&pctx_nv.pwm_range[PWM_OUT12].max  			,E_VA_UINT_FREE  },
  { "pwm_out12_delay", 	  		&pctx_nv.pwm_range[PWM_OUT12].delay_ms  	,E_VA_UINT_FREE  },

  { "motion_axis_mask", 		&pctx_nv.axis_used_mask						,E_VA_UINT_FREE  },
  { "io_cpu_dir", 				&pctx_nv.io_cpu_dir 						,E_VA_UINT_FREE  },
  { "in_cpu_pullup",			&pctx_nv.in_cpu_pullup						,E_VA_UINT_FREE  },
  { "in_cpu_filter",			&pctx_nv.in_cpu_filter						,E_VA_UINT_FREE  },
  { "io_rev_mask", 				&pctx_nv.io_rev_mask 						,E_VA_UINT_FREE  },
  { "dir_rev_mask",				&pctx_nv.dir_rev_mask						,E_VA_UINT_FREE  }

};




void params_init_default(void)
{
	int32_t ii;

	memset(&pctx_nv,0,sizeof(pctx_nv));

	for(ii = 0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		pctx_nv.axis[ii].pulses_step_m				= 20000;
		pctx_nv.axis[ii].pulses_enc_m  				= 0;

		pctx_nv.axis[ii].endpos_min_mask			= 0;
		pctx_nv.axis[ii].endpos_max_mask			= 0;

		pctx_nv.axis[ii].endpos_min_value			= 0;
		pctx_nv.axis[ii].endpos_max_value   		= 0;

		pctx_nv.axis[ii].homing_type 				= 0;
		pctx_nv.axis[ii].homing_mask    			= 0;
		pctx_nv.axis[ii].homing_value   			= 0;
		pctx_nv.axis[ii].homing_retract_mm 			= 0.0;


		pctx_nv.axis[ii].speed_mm_s					= 1000;
		pctx_nv.axis[ii].speed_home_mm_s			= 200;
		pctx_nv.axis[ii].speed_home_retract_mm_s	= 20;
		pctx_nv.axis[ii].speed_safe_mm_s 			= 100;

		pctx_nv.axis[ii].accel_mm_s2				= 1000;
		pctx_nv.axis[ii].jerk_mm_s3					= 10000;
	}


	for(ii = 0; ii < PWM_GLOBAL_CNT;ii++)
	{
		pctx_nv.pwm_range[ii].min   	= 0;
		pctx_nv.pwm_range[ii].max   	= 100;
		pctx_nv.pwm_range[ii].delay_ms  = 100;
	}


	/* Initial path settings */
	pctx_nv.path_initial_m.speed_mm_s    	= 1000;
	pctx_nv.path_initial_m.accel_mm_s2   	= 1000;
	pctx_nv.path_initial_m.jerk_mm_s3    	= 10000;

	/* Axis enabled */
	pctx_nv.axis_used_mask					= (1<< AXIS_X) | (1<<AXIS_Y) | (1<< AXIS_Z);

	/* IO_CPUxx direction */

	pctx_nv.io_cpu_dir						= 0;
	pctx_nv.io_rev_mask 					= 0;
	pctx_nv.dir_rev_mask					= 0;

	/* ESTOP */

	pctx_nv.estop_mask						= 0;

}

void params_init(void)
{
	memset(&pctx,0,sizeof(pctx));

	srv_nov_register(&pctx_nv,sizeof(pctx_nv),params_init_default);
	srv_sermon_register(params_var_ptable,sizeof(params_var_ptable));
}


void params_thread(void)
{

}


void params_once(void)
{
    mwr_periodic_low_register(params_thread,1000);
}


void	params_dump(int mode)
{
	int 			ii;

	int				resp_len;

	for(ii=0;ii < DIM(params_var_ptable);ii++)
	{
		resp_len = burst_process_variable(params_var_ptable[ii].name,NULL,pctx.resp_value,sizeof(pctx.resp_value)-1,NULL);

		if(resp_len > 0)
		{
			burst_log_printf(1,"%s=%s\r\n",params_var_ptable[ii].name,pctx.resp_value);
		}
	}
}




