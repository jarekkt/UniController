#include "motion_engine.h"




uint32_t motion_engine_step_axis(int32_t axis_idx,motion_buffer_t ** p_mbfr,int32_t * pulse_pos,int32_t * dir,int32_t * active_dir,int32_t * active)
{
	uint64_t 		  prev_accu;
	uint32_t 		  pulse = 0;
	motion_buffer_t * mbfr = *p_mbfr;

	if(mbfr != NULL)
	{
		if(mbfr->mf.tick_delay > 0)
		{
			mbfr->mf.tick_delay--;
			*active = 1;
		}
		else if(mbfr->mf.pulse_count > 0)
		{
			prev_accu = mbfr->mf.accu;
			mbfr->mf.accel_fract += mbfr->mf.jerk_fract;
			mbfr->mf.speed_fract += mbfr->mf.accel_fract;
			mbfr->mf.accu += mbfr->mf.speed_fract;

			if(prev_accu >  mbfr->mf.accu)
			{
				pulse = 1;
				mbfr->mf.pulse_count--;

				*pulse_pos += mbfr->dir;
			}

			if( mbfr->mf.pulse_count > 0)
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
				*dir    = (*p_mbfr)->dir;
				motion_engine_dir(axis_idx,*dir,active_dir);
				*active = 1;
			}
		}
	}

	return pulse;
}



void motion_engine_dir(int32_t idx,int32_t dir,int32_t * active_dir)
{
	dir = dir ^ ppctx_nv->dir_rev_mask;
	active_dir[idx] = dir;

	switch(idx)
	{
			case AXIS_X:
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
					GPIO_Set(OUT_DIR3);
				}
				else
				{
					GPIO_Clr(OUT_DIR3);
				}
			}break;

			case AXIS_A:
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
					GPIO_Set(OUT_DIR6);
				}
				else
				{
					GPIO_Clr(OUT_DIR6);
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





