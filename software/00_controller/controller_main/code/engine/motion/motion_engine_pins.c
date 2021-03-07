#include "motion_engine.h"

#define  HIT_LIMIT		5


static uint32_t motion_engine_step_axis(motion_buffer_t * mbfr,int32_t * pulse_pos,int32_t * active)
{
	uint64_t prev_accu;
	uint32_t pulse = 0;

next:
	if(mbfr->mf[mbfr->phase].pulse_count > 0)
	{
		prev_accu = mbfr->accu;
		mbfr->mf[mbfr->phase].accel_fract += mbfr->mf[mbfr->phase].jerk_fract;
		mbfr->mf[mbfr->phase].speed_fract += mbfr->mf[mbfr->phase].accel_fract;
		mbfr->accu += mbfr->mf[mbfr->phase].speed_fract;

		if(prev_accu >  mbfr->accu)
		{
			pulse = 1;
			mbfr->mf[mbfr->phase].pulse_count--;

			*pulse_pos += mbfr->dir;
		}
		*active = 1;
	}
	else
	{
		mbfr->accu = 0;
		if(mbfr->phase != MF_PHASES_CNT - 1)
		{
			mbfr->phase++;
			goto next;
		}
	}

	return pulse;
}



void motion_engine_dir(int32_t idx,int32_t dir)
{
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
		}
}




int32_t  motion_engine_tmr_pins(int32_t * pulse_pos,motion_buffer_t * mbx,motion_buffer_t * mby,motion_buffer_t * mbz)
{
	int32_t active = 0;

	if(mbx != NULL)
	{
		if(motion_engine_step_axis(mbx,pulse_pos++,&active) != 0)
		{
			TMR_TIRGGER_X();
		}
	}

	if(mby != NULL)
	{
		if( motion_engine_step_axis(mby,pulse_pos++,&active)!=0)
		{
			TMR_TIRGGER_Y();
		}
	}

	if(mbz != NULL)
	{
		if(motion_engine_step_axis(mbz,pulse_pos++,&active)!=0)
		{
			TMR_TIRGGER_Z();
		}
	}

	return active;
}


