#include "motion_engine.h"




uint32_t motion_engine_step_axis(int32_t axis_idx,motion_buffer_t ** p_mbfr,int32_t * pulse_pos,int32_t * dir,int32_t * active_dir,int32_t * active)
{
	//uint64_t 		  prev_accu;
	uint32_t 		  pulse = 0;
	motion_buffer_t * mbfr = *p_mbfr;

	if(mbfr != NULL)
	{
		if(mbfr->data.mf.tick_delay > 0)
		{
			mbfr->data.mf.tick_delay--;
			*active = 1;
		}
		else if(mbfr->data.mf.pulse_count > 0)
		{
			//prev_accu = mbfr->data.mf.accu;
			mbfr->data.mf.accel_fract += mbfr->data.mf.jerk_fract;
			mbfr->data.mf.speed_fract += mbfr->data.mf.accel_fract;
			mbfr->data.mf.accu 		  += mbfr->data.mf.speed_fract;

			if( mbfr->data.mf.accu >= 1.0)
			{
				mbfr->data.mf.accu = mbfr->data.mf.accu -1;

				pulse = 1;
				mbfr->data.mf.pulse_count--;

				*pulse_pos += mbfr->data.dir;
			}

			if( mbfr->data.mf.pulse_count > 0)
			{
				*active = 1;
			}
			else
			{
				goto next;
			}
		}
		else
		{
next:
			*p_mbfr = mbfr->next;
			if(*p_mbfr!= NULL)
			{
				*dir    = (*p_mbfr)->data.dir;
				motion_engine_dir(axis_idx,*dir,active_dir);
				*active = 1;
			}
		}
	}

	return pulse;
}



void motion_engine_dir(int32_t idx,int32_t dir,int32_t * active_dir)
{

	fw_assert(dir != 0);

	active_dir[idx] = dir;


	if( ppctx_nv->dir_rev_mask & (1<<idx))
	{
		dir = -dir;
	}

	switch(idx)
	{
			case AXIS_X:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR3);
				}
				else
				{
					GPIO_Clr(OUT_DIR3);
				}
			}break;

			case AXIS_Y:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR2);
				}
				else
				{
					GPIO_Clr(OUT_DIR2);
				}

			}break;

			case AXIS_Z:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_STEP6);
				}
				else
				{
					GPIO_Clr(OUT_STEP6);
				}


			}break;

			case AXIS_A:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR1);
				}
				else
				{
					GPIO_Clr(OUT_DIR1);
				}
			}break;

			case AXIS_B:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR5);
				}
				else
				{
					GPIO_Clr(OUT_DIR5);
				}
			}break;

			case AXIS_C:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR4);
				}
				else
				{
					GPIO_Clr(OUT_DIR4);
				}
			}break;

			case AXIS_D:
			{
				if(dir > 0)
				{
					GPIO_Set(OUT_DIR7);
				}
				else
				{
					GPIO_Clr(OUT_DIR7);
				}
			}break;
		}
}





