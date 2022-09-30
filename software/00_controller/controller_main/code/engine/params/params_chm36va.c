/*!
    \file params_chm36va.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "params.h"
#include "params_prv.h"



/* Hardware mapping :

	Outputs:

	- OUT_LED_DN				CPU1_PWM
	- OUT_LED_UP 				CPU2_PWM
	- PUMP_PRESS				CPU3_PWM
	- PUMP_VACUUM				CPU4_PWM
	- OUT_N1_VALVE				CPU_OUT6
	- OUT_N2_VALVE				CPU_OUT5
	- OUT_LZ 					CPU_OUT12_PWM
	- CAM_SWITCH				CPU_OUT7
	- ENA_XYZ					IO_CPU1
	- ENA_BCD					IO_CPU2

	Inputs:

	- IN_X_LIM					IN_CPU1
	- IN_Y_LIM					IN_CPU2
	- IN_Z_LIM					IN_CPU3
	- IN_LZ_LIM					IN_CPU4
	- ESTOP						IN_CPU5
	- IN1_ANA					ADC.CH1
	- IN2_ANA					ADC.CH2

 */



void	params_chm36va(void)
{

   // Machine X axis

   pctx_nv.axis[AXIS_Z].pulses_step_m     = 			31940;
   pctx_nv.axis[AXIS_Z].pulses_enc_m 	  = 				0; //TODO
   pctx_nv.axis[AXIS_Z].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_Z].endpos_max_mask   = 		P_IN_CPU1;
   pctx_nv.axis[AXIS_Z].endpos_min_value  =   	  		  0.0;
   pctx_nv.axis[AXIS_Z].endpos_max_value  = 			525.0;
   pctx_nv.axis[AXIS_Z].homing_type 	  =   P_HOMING_TO_MIN;
   pctx_nv.axis[AXIS_Z].homing_mask    	  = 		P_IN_CPU1;
   pctx_nv.axis[AXIS_Z].homing_value   	  =	    		  0.0;
   pctx_nv.axis[AXIS_Z].speed_mm_s        =    		   1000.0;
   pctx_nv.axis[AXIS_Z].speed_home_mm_s   = 			 80.0;
   pctx_nv.axis[AXIS_Z].speed_safe_mm_s   = 	 		 10.0;
   pctx_nv.axis[AXIS_Z].accel_mm_s2       =   		   4000.0;
   pctx_nv.axis[AXIS_Z].jerk_mm_s3        = 		1000000.0;

   // Machine Y axis

   pctx_nv.axis[AXIS_Y].pulses_step_m     = 			31940;
   pctx_nv.axis[AXIS_Y].pulses_enc_m 	  = 				0; //TODO
   pctx_nv.axis[AXIS_Y].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_Y].endpos_max_mask   = 		P_IN_CPU2;
   pctx_nv.axis[AXIS_Y].endpos_min_value  =   	  		  0.0;
   pctx_nv.axis[AXIS_Y].endpos_max_value  = 			475.0;
   pctx_nv.axis[AXIS_Y].homing_type 	  =   P_HOMING_TO_MIN;
   pctx_nv.axis[AXIS_Y].homing_mask    	  = 		P_IN_CPU2;
   pctx_nv.axis[AXIS_Y].homing_value   	  =	    		  0.0;
   pctx_nv.axis[AXIS_Y].speed_mm_s        =    		    800.0;
   pctx_nv.axis[AXIS_Y].speed_home_mm_s   = 			 80.0;
   pctx_nv.axis[AXIS_Y].speed_safe_mm_s   = 	 		 10.0;
   pctx_nv.axis[AXIS_Y].accel_mm_s2       =   		   4000.0;
   pctx_nv.axis[AXIS_Y].jerk_mm_s3        = 		 100000.0;

   // Machine Z axis

   pctx_nv.axis[AXIS_Z].pulses_step_m     = 		     4444;
   pctx_nv.axis[AXIS_Z].pulses_enc_m 	  = 				0;
   pctx_nv.axis[AXIS_Z].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_Z].endpos_max_mask   = 				0;
   pctx_nv.axis[AXIS_Z].endpos_min_value  =   	  		-45.0;
   pctx_nv.axis[AXIS_Z].endpos_max_value  = 			 45.0;
   pctx_nv.axis[AXIS_Z].homing_type 	  =   P_HOMING_TO_MID;
   pctx_nv.axis[AXIS_Z].homing_mask    	  = 		P_IN_CPU3;
   pctx_nv.axis[AXIS_Z].homing_value   	  =	    			0;
   pctx_nv.axis[AXIS_Z].speed_mm_s        =    		    800.0;
   pctx_nv.axis[AXIS_Z].speed_home_mm_s   = 			 80.0;
   pctx_nv.axis[AXIS_Z].speed_safe_mm_s   = 	 		 10.0;
   pctx_nv.axis[AXIS_Z].accel_mm_s2       =   		   8000.0;
   pctx_nv.axis[AXIS_Z].jerk_mm_s3        = 		 100000.0;

   // Machine N1 axis

   pctx_nv.axis[AXIS_A].pulses_step_m     = 				0;
   pctx_nv.axis[AXIS_A].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_A].endpos_max_mask   = 				0;
   pctx_nv.axis[AXIS_A].endpos_min_value  = 		   -120.0;
   pctx_nv.axis[AXIS_A].endpos_max_value  = 			120.0;
   pctx_nv.axis[AXIS_A].homing_type 	  = 	P_HOMING_NONE;
   pctx_nv.axis[AXIS_A].homing_mask    	  = 				0;
   pctx_nv.axis[AXIS_A].homing_value   	  = 			  0.0;
   pctx_nv.axis[AXIS_A].speed_mm_s        = 		   2000.0;
   pctx_nv.axis[AXIS_A].speed_home_mm_s   = 				0;
   pctx_nv.axis[AXIS_A].speed_safe_mm_s   = 			 10.0;
   pctx_nv.axis[AXIS_A].accel_mm_s2       = 		 100000.0;
   pctx_nv.axis[AXIS_A].jerk_mm_s3        = 	    1000000.0;

   // Machine N2 axis

   pctx_nv.axis[AXIS_B].pulses_step_m     = 				0;
   pctx_nv.axis[AXIS_B].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_B].endpos_max_mask   = 				0;
   pctx_nv.axis[AXIS_B].endpos_min_value  = 		   -120.0;
   pctx_nv.axis[AXIS_B].endpos_max_value  = 			120.0;
   pctx_nv.axis[AXIS_B].homing_type 	  = 	P_HOMING_NONE;
   pctx_nv.axis[AXIS_B].homing_mask    	  = 				0;
   pctx_nv.axis[AXIS_B].homing_value   	  = 			  0.0;
   pctx_nv.axis[AXIS_B].speed_mm_s        = 		   2000.0;
   pctx_nv.axis[AXIS_B].speed_home_mm_s   = 				0;
   pctx_nv.axis[AXIS_B].speed_safe_mm_s   = 			 10.0;
   pctx_nv.axis[AXIS_B].accel_mm_s2       = 		 100000.0;
   pctx_nv.axis[AXIS_B].jerk_mm_s3        = 	    1000000.0;

   // Machine peeler axis

   pctx_nv.axis[AXIS_C].pulses_step_m     = 		  69264.2;
   pctx_nv.axis[AXIS_C].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_C].endpos_max_mask   = 				0;
   pctx_nv.axis[AXIS_C].endpos_min_value  = 				0;
   pctx_nv.axis[AXIS_C].endpos_max_value  = 				0;
   pctx_nv.axis[AXIS_C].homing_type 	  = 	P_HOMING_NONE;
   pctx_nv.axis[AXIS_C].homing_mask    	  = 				0;
   pctx_nv.axis[AXIS_C].homing_value   	  = 				0;
   pctx_nv.axis[AXIS_C].speed_mm_s        = 		    500.0;
   pctx_nv.axis[AXIS_C].speed_home_mm_s   = 				0;
   pctx_nv.axis[AXIS_C].speed_safe_mm_s   = 			 10.0;
   pctx_nv.axis[AXIS_C].accel_mm_s2       = 		   1000.0;
   pctx_nv.axis[AXIS_C].jerk_mm_s3        = 		1000000.0;

   // Unused

   pctx_nv.axis[AXIS_D].pulses_step_m     = 				0;
   pctx_nv.axis[AXIS_D].endpos_min_mask   = 				0;
   pctx_nv.axis[AXIS_D].endpos_max_mask   = 				0;
   pctx_nv.axis[AXIS_D].endpos_min_value  = 				0;
   pctx_nv.axis[AXIS_D].endpos_max_value  = 				0;
   pctx_nv.axis[AXIS_D].homing_type 	  = 				0;
   pctx_nv.axis[AXIS_D].homing_mask    	  = 				0;
   pctx_nv.axis[AXIS_D].homing_value   	  = 				0;
   pctx_nv.axis[AXIS_D].speed_mm_s        = 				0;
   pctx_nv.axis[AXIS_D].speed_home_mm_s   = 				0;
   pctx_nv.axis[AXIS_D].speed_safe_mm_s   = 				0;
   pctx_nv.axis[AXIS_D].accel_mm_s2       = 				0;
   pctx_nv.axis[AXIS_D].jerk_mm_s3        = 				0;


   pctx_nv.path_initial_m.speed_mm_s      = 		   1000.0;
   pctx_nv.path_initial_m.accel_mm_s2     =    		  10000.0;
   pctx_nv.path_initial_m.jerk_mm_s3      =  		1000000.0;


   // LED DOWN
   pctx_nv.pwm_range[PWM_OUT1].min    	  = 				0;
   pctx_nv.pwm_range[PWM_OUT1].max  	  = 			  100;
   pctx_nv.pwm_range[PWM_OUT1].delay_ms   = 				0;

   // LED UP
   pctx_nv.pwm_range[PWM_OUT2].min    	  = 				0;
   pctx_nv.pwm_range[PWM_OUT2].max  	  = 			  100;
   pctx_nv.pwm_range[PWM_OUT2].delay_ms   = 				0;

   // PRESURE PUMP - operates at 2% like in original firmware
   pctx_nv.pwm_range[PWM_OUT3].min    	  = 				0;
   pctx_nv.pwm_range[PWM_OUT3].max  	  = 			   50;
   pctx_nv.pwm_range[PWM_OUT3].delay_ms   = 				0;

   // VAACUM PUMP - needs 50% for stable long term operation
   pctx_nv.pwm_range[PWM_OUT4].min    	  = 				0;
   pctx_nv.pwm_range[PWM_OUT4].max  	  = 		       50;
   pctx_nv.pwm_range[PWM_OUT4].delay_ms   = 				0;

   // UNUSED
   pctx_nv.pwm_range[PWM_OUT11].min   	  = 				0;
   pctx_nv.pwm_range[PWM_OUT11].max  	  = 				0;
   pctx_nv.pwm_range[PWM_OUT11].delay_ms  = 				0;

   // DRAG PIN ( LZ)
   // Will burn if operated at 100% for long – should be switched to 10%)
   // Here configured for 100ms full power and 10% PWM afterwards
   pctx_nv.pwm_range[PWM_OUT12].min   	  = 				0;
   pctx_nv.pwm_range[PWM_OUT12].max  	  = 			   10;
   pctx_nv.pwm_range[PWM_OUT12].delay_ms  = 			  100;

   pctx_nv.axis_used_mask			      = 	 (1<<AXIS_X) |
		   	   	   	   	   	   	   	   	   	   	 (1<<AXIS_Y) |
											     (1<<AXIS_Z) |
												 (1<<AXIS_A) |
												 (1<<AXIS_B) |
												 (1<<AXIS_C);

   // ESTOP configuration
   pctx_nv.estop_mask					  = 		P_IN_CPU5;

   // These will be used as outputs
   pctx_nv.io_cpu_dir 					  = 		P_IO_CPU1 |
		   	   	   	   	   	   	   	   	   	   	    P_IO_CPU2;

   // Pullup needed only for ESTOP (pull-downs are default, ESTOP uses reversed logic  )
   pctx_nv.in_cpu_pullup                  = 		P_IN_CPU5;

   // Reversed logic for inputs
   pctx_nv.io_rev_mask 					  = 				0;

   // Reverse dir pins
   pctx_nv.dir_rev_mask					  = 				0;

};



