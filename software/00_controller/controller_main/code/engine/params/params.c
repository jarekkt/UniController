/*!
    \file motion_engine.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "params.h"




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
  { "x_speed_mm_s",   			&pctx_nv.axis[AXIS_X].speed_mm_s       		,E_VA_FLT_FREE   },
  { "x_speed_home_mm_s", 		&pctx_nv.axis[AXIS_X].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "x_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_X].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "x_accel_mm_s2",   			&pctx_nv.axis[AXIS_X].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "x_jerk_mm_s3",   			&pctx_nv.axis[AXIS_X].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "y_pulses_step_m",   		&pctx_nv.axis[AXIS_Y].pulses_step_m     	,E_VA_UINT_FREE  },
  { "y_pulses_enc_m",		   	&pctx_nv.axis[AXIS_Y].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "y_endpos_min_mask",   		&pctx_nv.axis[AXIS_Y].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_max_mask",   		&pctx_nv.axis[AXIS_Y].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_min_value",   	&pctx_nv.axis[AXIS_Y].endpos_min_value      ,E_VA_FLT_FREE   },
  { "y_endpos_max_value",   	&pctx_nv.axis[AXIS_Y].endpos_max_value      ,E_VA_FLT_FREE   },
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
  { "z_speed_mm_s",  			&pctx_nv.axis[AXIS_Z].speed_mm_s       		,E_VA_FLT_FREE   },
  { "z_speed_home_mm_s", 		&pctx_nv.axis[AXIS_Z].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "z_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_Z].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "z_accel_mm_s2",   			&pctx_nv.axis[AXIS_Z].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "z_jerk_mm_s3",   			&pctx_nv.axis[AXIS_Z].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "a_pulses_step_m",		   	&pctx_nv.axis[AXIS_A].pulses_step_m     	,E_VA_UINT_FREE  },
  { "a_endpos_min_mask",   		&pctx_nv.axis[AXIS_A].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_max_mask",   		&pctx_nv.axis[AXIS_A].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_min_value",   	&pctx_nv.axis[AXIS_A].endpos_min_value      ,E_VA_FLT_FREE   },
  { "a_endpos_max_value",   	&pctx_nv.axis[AXIS_A].endpos_max_value      ,E_VA_FLT_FREE   },
  { "a_speed_mm_s",  			&pctx_nv.axis[AXIS_A].speed_mm_s       		,E_VA_FLT_FREE   },
  { "a_speed_home_mm_s", 		&pctx_nv.axis[AXIS_A].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "a_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_A].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "a_accel_mm_s2",   			&pctx_nv.axis[AXIS_A].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "a_jerk_mm_s3",   			&pctx_nv.axis[AXIS_A].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "b_pulses_step_m",   		&pctx_nv.axis[AXIS_B].pulses_step_m     	,E_VA_UINT_FREE  },
  { "b_endpos_min_mask",   		&pctx_nv.axis[AXIS_B].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_max_mask",   		&pctx_nv.axis[AXIS_B].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_min_value",   	&pctx_nv.axis[AXIS_B].endpos_min_value      ,E_VA_FLT_FREE   },
  { "b_endpos_max_value",   	&pctx_nv.axis[AXIS_B].endpos_max_value      ,E_VA_FLT_FREE   },
  { "b_speed_mm_s",   			&pctx_nv.axis[AXIS_B].speed_mm_s       		,E_VA_FLT_FREE   },
  { "b_speed_home_mm_s", 		&pctx_nv.axis[AXIS_B].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "b_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_B].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "b_accel_mm_s2",   			&pctx_nv.axis[AXIS_B].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "b_jerk_mm_s3",   			&pctx_nv.axis[AXIS_B].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "c_pulses_step_m",   		&pctx_nv.axis[AXIS_C].pulses_step_m     	,E_VA_UINT_FREE  },
  { "c_endpos_min_mask",   		&pctx_nv.axis[AXIS_C].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "c_endpos_max_mask",   		&pctx_nv.axis[AXIS_C].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "c_endpos_min_value",   	&pctx_nv.axis[AXIS_C].endpos_min_value      ,E_VA_FLT_FREE   },
  { "c_endpos_max_value",   	&pctx_nv.axis[AXIS_C].endpos_max_value      ,E_VA_FLT_FREE   },
  { "c_speed_mm_s", 			&pctx_nv.axis[AXIS_C].speed_mm_s       		,E_VA_FLT_FREE   },
  { "c_speed_home_mm_s", 		&pctx_nv.axis[AXIS_C].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "c_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_C].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "c_accel_mm_s2",   			&pctx_nv.axis[AXIS_C].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "c_jerk_mm_s3",   			&pctx_nv.axis[AXIS_C].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "d_pulses_step_m",		   	&pctx_nv.axis[AXIS_D].pulses_step_m     	,E_VA_UINT_FREE  },
  { "d_endpos_min_mask",   		&pctx_nv.axis[AXIS_D].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_max_mask",   		&pctx_nv.axis[AXIS_D].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_min_value",   	&pctx_nv.axis[AXIS_D].endpos_min_value      ,E_VA_FLT_FREE   },
  { "d_endpos_max_value",   	&pctx_nv.axis[AXIS_D].endpos_max_value      ,E_VA_FLT_FREE   },
  { "d_speed_mm_s",  			&pctx_nv.axis[AXIS_D].speed_mm_s       		,E_VA_FLT_FREE   },
  { "d_speed_home_mm_s", 		&pctx_nv.axis[AXIS_D].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "d_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_D].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "d_accel_mm_s2",   			&pctx_nv.axis[AXIS_D].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "d_jerk_mm_s3",   			&pctx_nv.axis[AXIS_D].jerk_mm_s3       		,E_VA_FLT_FREE   },
  { "d_pulses_step_m",		   	&pctx_nv.axis[AXIS_D].pulses_step_m     	,E_VA_UINT_FREE  },
  { "d_pulses_enc_m",   		&pctx_nv.axis[AXIS_D].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "d_endpos_min_mask",   		&pctx_nv.axis[AXIS_D].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_max_mask",   		&pctx_nv.axis[AXIS_D].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "d_endpos_min_value",   	&pctx_nv.axis[AXIS_D].endpos_min_value      ,E_VA_FLT_FREE   },
  { "d_endpos_max_value",   	&pctx_nv.axis[AXIS_D].endpos_max_value      ,E_VA_FLT_FREE   },
  { "d_speed_mm_s",  			&pctx_nv.axis[AXIS_D].speed_mm_s       		,E_VA_FLT_FREE   },
  { "d_speed_home_mm_s", 		&pctx_nv.axis[AXIS_D].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "d_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_D].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "d_accel_mm_s2",   			&pctx_nv.axis[AXIS_D].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "d_jerk_mm_s3",   			&pctx_nv.axis[AXIS_D].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "p_speed_mm_s",  			&pctx_nv.path_initial_m.speed_mm_s      	,E_VA_FLT_FREE   },
  { "p_accel_mm_s2",   			&pctx_nv.path_initial_m.accel_mm_s2     	,E_VA_FLT_FREE   },
  { "p_jerk_mm_s3",   			&pctx_nv.path_initial_m.jerk_mm_s3      	,E_VA_FLT_FREE   },

  { "motion_axis_mask", 		&pctx_nv.axis_used_mask						,E_VA_UINT_FREE  },
};




void params_init_default(void)
{
	int32_t ii;

	memset(&pctx_nv,0,sizeof(pctx_nv));

	for(ii = 0; ii < AXIS_GLOBAL_CNT;ii++)
	{
		pctx_nv.axis[ii].pulses_step_m		= 20000;
		pctx_nv.axis[ii].pulses_enc_m  		= 0;

		pctx_nv.axis[ii].endpos_min_mask	= 0x01;
		pctx_nv.axis[ii].endpos_max_mask	= 0x04;

		pctx_nv.axis[ii].endpos_min_value	= 0;
		pctx_nv.axis[ii].endpos_max_value   = 500;

		pctx_nv.axis[ii].speed_mm_s			= 1000;
		pctx_nv.axis[ii].speed_home_mm_s	= 200;
		pctx_nv.axis[ii].speed_safe_mm_s 	= 100;

		pctx_nv.axis[ii].accel_mm_s2		= 1000;
		pctx_nv.axis[ii].jerk_mm_s3			= 10000;
	}


	/* Initial path settings */
	pctx_nv.path_initial_m.speed_mm_s    	= 1000;
	pctx_nv.path_initial_m.accel_mm_s2   	= 1000;
	pctx_nv.path_initial_m.jerk_mm_s3    	= 10000;

	/* Axis enabled */
	pctx_nv.axis_used_mask					= (1<< AXIS_X) | (1<<AXIS_Y) | (1<< AXIS_Z);

}

void params_init(void)
{
	srv_nov_register(&pctx_nv,sizeof(pctx_nv),params_init_default);
	srv_sermon_register(params_var_ptable,sizeof(params_var_ptable));
}


void params_once(void)
{

}

