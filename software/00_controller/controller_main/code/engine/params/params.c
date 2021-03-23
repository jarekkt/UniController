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
  { "x_endpos_park_mask",   	&pctx_nv.axis[AXIS_X].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "x_endpos_max_mask",   		&pctx_nv.axis[AXIS_X].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "x_endpos_min_value",   	&pctx_nv.axis[AXIS_X].endpos_min_value      ,E_VA_FLT_FREE   },
  { "x_endpos_park_value",   	&pctx_nv.axis[AXIS_X].endpos_park_value     ,E_VA_FLT_FREE   },
  { "x_endpos_max_value",   	&pctx_nv.axis[AXIS_X].endpos_max_value      ,E_VA_FLT_FREE   },
  { "x_speed_mm_s",   			&pctx_nv.axis[AXIS_X].speed_mm_s       		,E_VA_FLT_FREE   },
  { "x_speed_home_mm_s", 		&pctx_nv.axis[AXIS_X].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "x_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_X].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "x_accel_mm_s2",   			&pctx_nv.axis[AXIS_X].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "x_jerk_mm_s3",   			&pctx_nv.axis[AXIS_X].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "y_pulses_step_m",   		&pctx_nv.axis[AXIS_Y].pulses_step_m     	,E_VA_UINT_FREE  },
  { "y_pulses_enc_m",		   	&pctx_nv.axis[AXIS_Y].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "y_endpos_min_mask",   		&pctx_nv.axis[AXIS_Y].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_park_mask",   	&pctx_nv.axis[AXIS_Y].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "y_endpos_may_mask",   		&pctx_nv.axis[AXIS_Y].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "y_endpos_min_value",   	&pctx_nv.axis[AXIS_Y].endpos_min_value      ,E_VA_FLT_FREE   },
  { "y_endpos_park_value",   	&pctx_nv.axis[AXIS_Y].endpos_park_value     ,E_VA_FLT_FREE   },
  { "y_endpos_may_value",   	&pctx_nv.axis[AXIS_Y].endpos_max_value      ,E_VA_FLT_FREE   },
  { "y_speed_mm_s", 			&pctx_nv.axis[AXIS_Y].speed_mm_s       		,E_VA_FLT_FREE   },
  { "y_speed_home_mm_s", 		&pctx_nv.axis[AXIS_Y].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "y_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_Y].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "y_accel_mm_s2",   			&pctx_nv.axis[AXIS_Y].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "y_jerk_mm_s3",   			&pctx_nv.axis[AXIS_Y].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "z_pulses_step_m",		   	&pctx_nv.axis[AXIS_Z].pulses_step_m     	,E_VA_UINT_FREE  },
  { "z_pulses_enc_m",   		&pctx_nv.axis[AXIS_Z].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "z_endpos_min_mask",   		&pctx_nv.axis[AXIS_Z].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "z_endpos_park_mask",   	&pctx_nv.axis[AXIS_Z].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "z_endpos_maz_mask",   		&pctx_nv.axis[AXIS_Z].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "z_endpos_min_value",   	&pctx_nv.axis[AXIS_Z].endpos_min_value      ,E_VA_FLT_FREE   },
  { "z_endpos_park_value",   	&pctx_nv.axis[AXIS_Z].endpos_park_value     ,E_VA_FLT_FREE   },
  { "z_endpos_maz_value",   	&pctx_nv.axis[AXIS_Z].endpos_max_value      ,E_VA_FLT_FREE   },
  { "z_speed_mm_s",  			&pctx_nv.axis[AXIS_Z].speed_mm_s       		,E_VA_FLT_FREE   },
  { "z_speed_home_mm_s", 		&pctx_nv.axis[AXIS_Z].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "z_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_Z].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "z_accel_mm_s2",   			&pctx_nv.axis[AXIS_Z].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "z_jerk_mm_s3",   			&pctx_nv.axis[AXIS_Z].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "a_pulses_step_m",		   	&pctx_nv.axis[AXIS_A].pulses_step_m     	,E_VA_UINT_FREE  },
  { "a_pulses_enc_m",   		&pctx_nv.axis[AXIS_A].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "a_endpos_min_mask",   		&pctx_nv.axis[AXIS_A].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_park_mask",   	&pctx_nv.axis[AXIS_A].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "a_endpos_maa_mask",   		&pctx_nv.axis[AXIS_A].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "a_endpos_min_value",   	&pctx_nv.axis[AXIS_A].endpos_min_value      ,E_VA_FLT_FREE   },
  { "a_endpos_park_value",   	&pctx_nv.axis[AXIS_A].endpos_park_value     ,E_VA_FLT_FREE   },
  { "a_endpos_maa_value",   	&pctx_nv.axis[AXIS_A].endpos_max_value      ,E_VA_FLT_FREE   },
  { "a_speed_mm_s",  			&pctx_nv.axis[AXIS_A].speed_mm_s       		,E_VA_FLT_FREE   },
  { "a_speed_home_mm_s", 		&pctx_nv.axis[AXIS_A].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "a_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_A].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "a_accel_mm_s2",   			&pctx_nv.axis[AXIS_A].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "a_jerk_mm_s3",   			&pctx_nv.axis[AXIS_A].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "u_pulses_step_m",   		&pctx_nv.axis[AXIS_U].pulses_step_m     	,E_VA_UINT_FREE  },
  { "u_pulses_enc_m", 		  	&pctx_nv.axis[AXIS_U].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "u_endpos_min_mask",   		&pctx_nv.axis[AXIS_U].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "u_endpos_park_mask",   	&pctx_nv.axis[AXIS_U].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "u_endpos_max_mask",   		&pctx_nv.axis[AXIS_U].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "u_endpos_min_value",   	&pctx_nv.axis[AXIS_U].endpos_min_value      ,E_VA_FLT_FREE   },
  { "u_endpos_park_value",   	&pctx_nv.axis[AXIS_U].endpos_park_value     ,E_VA_FLT_FREE   },
  { "u_endpos_max_value",   	&pctx_nv.axis[AXIS_U].endpos_max_value      ,E_VA_FLT_FREE   },
  { "u_speed_mm_s",   			&pctx_nv.axis[AXIS_U].speed_mm_s       		,E_VA_FLT_FREE   },
  { "u_speed_home_mm_s", 		&pctx_nv.axis[AXIS_U].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "u_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_U].speed_safe_mm_s   	,E_VA_FLT_FREE   },
  { "u_accel_mm_s2",   			&pctx_nv.axis[AXIS_U].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "u_jerk_mm_s3",   			&pctx_nv.axis[AXIS_U].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "v_pulses_step_m",   		&pctx_nv.axis[AXIS_V].pulses_step_m     	,E_VA_UINT_FREE  },
  { "v_pulses_enc_m",		   	&pctx_nv.axis[AXIS_V].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "v_endpos_min_mask",   		&pctx_nv.axis[AXIS_V].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "v_endpos_park_mask",   	&pctx_nv.axis[AXIS_V].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "v_endpos_may_mask",   		&pctx_nv.axis[AXIS_V].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "v_endpos_min_value",   	&pctx_nv.axis[AXIS_V].endpos_min_value      ,E_VA_FLT_FREE   },
  { "v_endpos_park_value",   	&pctx_nv.axis[AXIS_V].endpos_park_value     ,E_VA_FLT_FREE   },
  { "v_endpos_may_value",   	&pctx_nv.axis[AXIS_V].endpos_max_value      ,E_VA_FLT_FREE   },
  { "v_speed_mm_s", 			&pctx_nv.axis[AXIS_V].speed_mm_s       		,E_VA_FLT_FREE   },
  { "v_speed_home_mm_s", 		&pctx_nv.axis[AXIS_V].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "v_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_V].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "v_accel_mm_s2",   			&pctx_nv.axis[AXIS_V].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "v_jerk_mm_s3",   			&pctx_nv.axis[AXIS_V].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "w_pulses_step_m",		   	&pctx_nv.axis[AXIS_W].pulses_step_m     	,E_VA_UINT_FREE  },
  { "w_pulses_enc_m",   		&pctx_nv.axis[AXIS_W].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "w_endpos_min_mask",   		&pctx_nv.axis[AXIS_W].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "w_endpos_park_mask",   	&pctx_nv.axis[AXIS_W].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "w_endpos_maz_mask",   		&pctx_nv.axis[AXIS_W].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "w_endpos_min_value",   	&pctx_nv.axis[AXIS_W].endpos_min_value      ,E_VA_FLT_FREE   },
  { "w_endpos_park_value",   	&pctx_nv.axis[AXIS_W].endpos_park_value     ,E_VA_FLT_FREE   },
  { "w_endpos_maz_value",   	&pctx_nv.axis[AXIS_W].endpos_max_value      ,E_VA_FLT_FREE   },
  { "w_speed_mm_s",  			&pctx_nv.axis[AXIS_W].speed_mm_s       		,E_VA_FLT_FREE   },
  { "w_speed_home_mm_s", 		&pctx_nv.axis[AXIS_W].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "w_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_W].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "w_accel_mm_s2",   			&pctx_nv.axis[AXIS_W].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "w_jerk_mm_s3",   			&pctx_nv.axis[AXIS_W].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "b_pulses_step_m",		   	&pctx_nv.axis[AXIS_B].pulses_step_m     	,E_VA_UINT_FREE  },
  { "b_pulses_enc_m",   		&pctx_nv.axis[AXIS_B].pulses_enc_m 		    ,E_VA_UINT_FREE  },
  { "b_endpos_min_mask",   		&pctx_nv.axis[AXIS_B].endpos_min_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_park_mask",   	&pctx_nv.axis[AXIS_B].endpos_park_mask      ,E_VA_UINT_FREE  },
  { "b_endpos_maa_mask",   		&pctx_nv.axis[AXIS_B].endpos_max_mask       ,E_VA_UINT_FREE  },
  { "b_endpos_min_value",   	&pctx_nv.axis[AXIS_B].endpos_min_value      ,E_VA_FLT_FREE   },
  { "b_endpos_park_value",   	&pctx_nv.axis[AXIS_B].endpos_park_value     ,E_VA_FLT_FREE   },
  { "b_endpos_maa_value",   	&pctx_nv.axis[AXIS_B].endpos_max_value      ,E_VA_FLT_FREE   },
  { "b_speed_mm_s",  			&pctx_nv.axis[AXIS_B].speed_mm_s       		,E_VA_FLT_FREE   },
  { "b_speed_home_mm_s", 		&pctx_nv.axis[AXIS_B].speed_home_mm_s  		,E_VA_FLT_FREE   },
  { "b_speed_safe_mm_s",   		&pctx_nv.axis[AXIS_B].speed_safe_mm_s    	,E_VA_FLT_FREE   },
  { "b_accel_mm_s2",   			&pctx_nv.axis[AXIS_B].accel_mm_s2       	,E_VA_FLT_FREE   },
  { "b_jerk_mm_s3",   			&pctx_nv.axis[AXIS_B].jerk_mm_s3       		,E_VA_FLT_FREE   },

  { "p_speed_mm_s",  			&pctx_nv.path_initial_m.speed_mm_s      	,E_VA_FLT_FREE   },
  { "p_accel_mm_s2",   			&pctx_nv.path_initial_m.accel_mm_s2     	,E_VA_FLT_FREE   },
  { "p_jerk_mm_s3",   			&pctx_nv.path_initial_m.jerk_mm_s3      	,E_VA_FLT_FREE   },

  { "motion_geometry",   		&pctx_nv.geometry  					    	,E_VA_UINT_FREE  },
};




void params_init_default(void)
{
	int32_t ii;

	memset(&pctx_nv,0,sizeof(pctx_nv));

	for(ii =0; ii < AXIS_CNT;ii++)
	{
		pctx_nv.axis[ii].pulses_step_m		= 20000;
		pctx_nv.axis[ii].pulses_enc_m  		= 0;

		pctx_nv.axis[ii].endpos_min_mask	= 0x01;
		pctx_nv.axis[ii].endpos_park_mask	= 0x02;
		pctx_nv.axis[ii].endpos_max_mask	= 0x04;

		pctx_nv.axis[ii].endpos_min_value	= 0;
		pctx_nv.axis[ii].endpos_park_value	= 0;
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

}

void params_init(void)
{
	srv_nov_register(&pctx_nv,sizeof(pctx_nv),params_init_default);
	srv_sermon_register(params_var_ptable,sizeof(params_var_ptable));
}


void params_once(void)
{

}

