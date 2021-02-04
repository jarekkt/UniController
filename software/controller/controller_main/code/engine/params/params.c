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
  { "x_pulses_step_100mm",   	&pctx_nv.axis[AXIS_X].pulses_step_100mm     ,E_VA_INT_FREE   },
  { "x_pulses_enc_100mm",   	&pctx_nv.axis[AXIS_X].pulses_enc_100mm      ,E_VA_INT_FREE   },
  { "x_endpos_min_mask",   		&pctx_nv.axis[AXIS_X].endpos_min_mask       ,E_VA_INT_FREE   },
  { "x_endpos_park_mask",   	&pctx_nv.axis[AXIS_X].endpos_park_mask      ,E_VA_INT_FREE   },
  { "x_endpos_max_mask",   		&pctx_nv.axis[AXIS_X].endpos_max_mask       ,E_VA_INT_FREE   },
  { "x_endpos_min_value",   	&pctx_nv.axis[AXIS_X].endpos_min_value      ,E_VA_INT_FREE   },
  { "x_endpos_park_value",   	&pctx_nv.axis[AXIS_X].endpos_park_value     ,E_VA_INT_FREE   },
  { "x_endpos_max_value",   	&pctx_nv.axis[AXIS_X].endpos_max_value      ,E_VA_INT_FREE   },
  { "x_speed_001mm_s",   		&pctx_nv.axis[AXIS_X].speed_001mm_s       	,E_VA_INT_FREE   },
  { "x_speed_home_001mm_s", 	&pctx_nv.axis[AXIS_X].speed_home_001mm_s  	,E_VA_INT_FREE   },
  { "x_speed_safe_001mm_s",   	&pctx_nv.axis[AXIS_X].speed_safe_001mm_s    ,E_VA_INT_FREE   },
  { "x_accel_001mm_s2",   		&pctx_nv.axis[AXIS_X].accel_001mm_s2       	,E_VA_INT_FREE   },
  { "x_jerk_001mm_s3",   		&pctx_nv.axis[AXIS_X].jerk_001mm_s3       	,E_VA_INT_FREE   },

  { "y_pulses_step_100mm",   	&pctx_nv.axis[AXIS_Y].pulses_step_100mm     ,E_VA_INT_FREE   },
  { "y_pulses_enc_100mm",   	&pctx_nv.axis[AXIS_Y].pulses_enc_100mm      ,E_VA_INT_FREE   },
  { "y_endpos_min_mask",   		&pctx_nv.axis[AXIS_Y].endpos_min_mask       ,E_VA_INT_FREE   },
  { "y_endpos_park_mask",   	&pctx_nv.axis[AXIS_Y].endpos_park_mask      ,E_VA_INT_FREE   },
  { "y_endpos_may_mask",   		&pctx_nv.axis[AXIS_Y].endpos_max_mask       ,E_VA_INT_FREE   },
  { "y_endpos_min_value",   	&pctx_nv.axis[AXIS_Y].endpos_min_value      ,E_VA_INT_FREE   },
  { "y_endpos_park_value",   	&pctx_nv.axis[AXIS_Y].endpos_park_value     ,E_VA_INT_FREE   },
  { "y_endpos_may_value",   	&pctx_nv.axis[AXIS_Y].endpos_max_value      ,E_VA_INT_FREE   },
  { "y_speed_001mm_s", 			&pctx_nv.axis[AXIS_Y].speed_001mm_s       	,E_VA_INT_FREE   },
  { "y_speed_home_001mm_s", 	&pctx_nv.axis[AXIS_Y].speed_home_001mm_s  	,E_VA_INT_FREE   },
  { "y_speed_safe_001mm_s",   	&pctx_nv.axis[AXIS_Y].speed_safe_001mm_s    ,E_VA_INT_FREE   },
  { "y_accel_001mm_s2",   		&pctx_nv.axis[AXIS_Y].accel_001mm_s2       	,E_VA_INT_FREE   },
  { "y_jerk_001mm_s3",   		&pctx_nv.axis[AXIS_Y].jerk_001mm_s3       	,E_VA_INT_FREE   },

  { "z_pulses_step_100mm",   	&pctx_nv.axis[AXIS_Z].pulses_step_100mm     ,E_VA_INT_FREE   },
  { "z_pulses_enc_100mm",   	&pctx_nv.axis[AXIS_Z].pulses_enc_100mm      ,E_VA_INT_FREE   },
  { "z_endpos_min_mask",   		&pctx_nv.axis[AXIS_Z].endpos_min_mask       ,E_VA_INT_FREE   },
  { "z_endpos_park_mask",   	&pctx_nv.axis[AXIS_Z].endpos_park_mask      ,E_VA_INT_FREE   },
  { "z_endpos_maz_mask",   		&pctx_nv.axis[AXIS_Z].endpos_max_mask       ,E_VA_INT_FREE   },
  { "z_endpos_min_value",   	&pctx_nv.axis[AXIS_Z].endpos_min_value      ,E_VA_INT_FREE   },
  { "z_endpos_park_value",   	&pctx_nv.axis[AXIS_Z].endpos_park_value     ,E_VA_INT_FREE   },
  { "z_endpos_maz_value",   	&pctx_nv.axis[AXIS_Z].endpos_max_value      ,E_VA_INT_FREE   },
  { "z_speed_001mm_s",  		&pctx_nv.axis[AXIS_Z].speed_001mm_s       	,E_VA_INT_FREE   },
  { "z_speed_home_001mm_s", 	&pctx_nv.axis[AXIS_Z].speed_home_001mm_s  	,E_VA_INT_FREE   },
  { "z_speed_safe_001mm_s",   	&pctx_nv.axis[AXIS_Z].speed_safe_001mm_s    ,E_VA_INT_FREE   },
  { "z_accel_001mm_s2",   		&pctx_nv.axis[AXIS_Z].accel_001mm_s2       	,E_VA_INT_FREE   },
  { "z_jerk_001mm_s3",   		&pctx_nv.axis[AXIS_Z].jerk_001mm_s3       	,E_VA_INT_FREE   },
};




void params_init_default(void)
{
	memset(&pctx_nv,0,sizeof(pctx_nv));

	/* X */
	pctx_nv.axis[AXIS_X].pulses_step_100mm	= 2000;
	pctx_nv.axis[AXIS_X].pulses_enc_100mm   = 0;

	pctx_nv.axis[AXIS_X].endpos_min_mask	= 0x01;
	pctx_nv.axis[AXIS_X].endpos_park_mask	= 0x02;
	pctx_nv.axis[AXIS_X].endpos_max_mask	= 0x04;

	pctx_nv.axis[AXIS_X].endpos_min_value	= 0;
	pctx_nv.axis[AXIS_X].endpos_park_value	= 0;
	pctx_nv.axis[AXIS_X].endpos_max_value   = 500000;

	pctx_nv.axis[AXIS_X].speed_001mm_s		= 500000;
	pctx_nv.axis[AXIS_X].speed_home_001mm_s	= 50000;
	pctx_nv.axis[AXIS_X].speed_safe_001mm_s = 5000;

	pctx_nv.axis[AXIS_X].accel_001mm_s2		= 500000;
	pctx_nv.axis[AXIS_X].jerk_001mm_s3		=  50000;

	/* Y */
	pctx_nv.axis[AXIS_Y].pulses_step_100mm	= 2000;
	pctx_nv.axis[AXIS_Y].pulses_enc_100mm   = 0;

	pctx_nv.axis[AXIS_Y].endpos_min_mask	= 0x01;
	pctx_nv.axis[AXIS_Y].endpos_park_mask	= 0x02;
	pctx_nv.axis[AXIS_Y].endpos_max_mask	= 0x04;

	pctx_nv.axis[AXIS_Y].endpos_min_value	= 0;
	pctx_nv.axis[AXIS_Y].endpos_park_value	= 0;
	pctx_nv.axis[AXIS_Y].endpos_max_value   = 500000;

	pctx_nv.axis[AXIS_Y].speed_001mm_s		= 500000;
	pctx_nv.axis[AXIS_Y].speed_home_001mm_s	= 50000;
	pctx_nv.axis[AXIS_Y].speed_safe_001mm_s = 5000;

	pctx_nv.axis[AXIS_Y].accel_001mm_s2		= 500000;
	pctx_nv.axis[AXIS_Y].jerk_001mm_s3		=  50000;


	/* Z */
	pctx_nv.axis[AXIS_Z].pulses_step_100mm	= 2000;
	pctx_nv.axis[AXIS_Z].pulses_enc_100mm   = 0;

	pctx_nv.axis[AXIS_Z].endpos_min_mask	= 0x01;
	pctx_nv.axis[AXIS_Z].endpos_park_mask	= 0x02;
	pctx_nv.axis[AXIS_Z].endpos_max_mask	= 0x04;

	pctx_nv.axis[AXIS_Z].endpos_min_value	= 0;
	pctx_nv.axis[AXIS_Z].endpos_park_value	= 0;
	pctx_nv.axis[AXIS_Z].endpos_max_value   = 500000;

	pctx_nv.axis[AXIS_Z].speed_001mm_s		= 500000;
	pctx_nv.axis[AXIS_Z].speed_home_001mm_s	= 50000;
	pctx_nv.axis[AXIS_Z].speed_safe_001mm_s = 5000;

	pctx_nv.axis[AXIS_Z].accel_001mm_s2		= 500000;
	pctx_nv.axis[AXIS_Z].jerk_001mm_s3		=  50000;



}

void params_init(void)
{
	srv_nov_register(&pctx_nv,sizeof(pctx_nv),params_init_default);
	srv_sermon_register(params_var_ptable,sizeof(params_var_ptable));
}


void params_once(void)
{

}

