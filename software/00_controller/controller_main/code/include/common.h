#ifndef COMMON_H
#define COMMON_H

typedef enum
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_FAST_CNT,
	AXIS_A = AXIS_FAST_CNT,
	AXIS_B,
	AXIS_C,
	AXIS_D,
	AXIS_E,
	AXIS_GLOBAL_CNT
}axis_idx_e;

typedef enum
{
	PWM_OUT1,
	PWM_OUT2,
	PWM_OUT3,
	PWM_OUT4,
	PWM_OUT11,
	PWM_OUT12,
	PWM_GLOBAL_CNT
}pwm_idx_e;



#endif //  COMMON_H
