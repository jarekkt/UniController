/*!
    \file motion_scurve.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"

#include "motion_scurve.h"
#include "motion_eq.h"


typedef struct
{
	 double m_dist;
	 double m_speed;
	 double m_speed0;
	 double m_accel;
	 double m_jerk;
	 double half_dist;
	 double calc_dist;
	 double calc_speed;

	 motion_calc_t * calc;
}self_t;

typedef struct
{
	 double distance;
	 double speed0;
	 double speed;
	 double accel;
	 double jerk;
}test_set_t;

typedef struct
{
	 double T11;
	 double T12;
	 double T13;
	 double T2;
	 double speed0;
	 double speed;
	 double accel;
	 double jerk;

}test_result_t;

typedef struct
{
	test_set_t    ts;
	test_result_t tr;

}test_case_t;


// Generated by python.py
const test_case_t test_cases[] = {
  {
	{
	20000.0/*dist*/, 500.0 /* speed0 */, 1500.0 /* speed */, 1000.0 /* accel */, 500.0 /* jerk */
	},
	{
	1.4142135623730951 /* T11 */, 0 /* T12 */, 1.4142135623730951 /* T13 */,  9.562097167005078 /* T2 */, 500.0 /* speed0 */, 1500.0 /* speed */, 707.1067811865476 /* accel */, 500.0 /* jerk */
	},
  },
  {
	{
	10.0/*dist*/, 5.0 /* speed0 */, 50.0 /* speed */, 50.0 /* accel */, 50.0 /* jerk */
	},
	{
	0.09950615177038495 /* T11 */, 0.5552665834129124 /* T12 */, 0.09950615177038495 /* T13 */,  0.0 /* T2 */, 5.0 /* speed0 */, 50.0 /* speed */, 4.9753075885192475 /* accel */, 50.0 /* jerk */
	},
  }
};

double solve_quad_positive(double a, double b, double c)
{
	int32_t	 eq_num;
	int32_t  ii;
	double   eq_results[3];

	 eq_num = quad_eq(a,b,c,eq_results);

	 if(eq_num > 0)
	 {
		for(ii=0;ii<eq_num;ii++)
		{
			if(eq_results[ii]>0)return eq_results[ii];
		}
		return 0;
	 }
	 else
	 {
		 return 0;
	 }
}


double solve_qubic_positive(double a, double b, double c,double d)
{
	int32_t	 eq_num;
	int32_t  ii;
	double   eq_results[3];

	eq_num = cubic_eq(a,b,c,d,eq_results);

	if(eq_num > 0)
	{
		for(ii=0;ii<eq_num;ii++)
		{
			if(eq_results[ii]>0)return eq_results[ii];
		}
	    return 0;
	}
	else
	{
	  return 0;
	}
}


void motion_scurve_calc(
		motion_calc_t * calc,
		float 			dist_mm,
		float 			speed_safe_mm_s,
		float 			speed_mm_s,
		float 			accel_mm_s2,
		float 			jerk_mm_s3
)
{
	 self_t		 self;

	 double		 a_scurve;
	 double		 top_speed;


	 if(dist_mm == 0)
	 {
		 memset(calc,0,sizeof(*calc));
	 }


	 if(dist_mm > 0)
	 {
		 calc->dir = 1;
	 }
	 else
	 {
		 calc->dir  = -1;
		 dist_mm = -dist_mm;
	 }


	 self.m_dist    = dist_mm;
	 self.m_speed   = speed_mm_s;
	 self.m_speed0  = speed_safe_mm_s;
	 self.m_accel   = accel_mm_s2;
	 self.m_jerk    = jerk_mm_s3;

	 self.half_dist = self.m_dist / 2;
	 self.calc 		= calc;


	 // Calculate ideal S-curve acceleration
	 a_scurve = solve_qubic_positive( 1/self.m_jerk,0,2*self.m_speed0,-self.half_dist*self.m_jerk);

	 if(a_scurve < self.m_accel)
	 {
		 //Changing acceleration (ideal s curve distance limit)
		 self.m_accel = a_scurve;
	 }

	 top_speed = self.m_speed0 + self.m_accel * self.m_accel/self.m_jerk;

	 if (top_speed > self.m_speed)
	 {
		 // Given acceleration makes too big speed - even for ideal S curve
		 self.m_accel = sqrt((self.m_speed - self.m_speed0)*self.m_jerk);
		 self.m_speed = self.m_speed0 + self.m_accel * self.m_accel/self.m_jerk;
	 }

	 self.calc->T11 = self.m_accel / self.m_jerk;
	 self.calc->T13 = self.calc->T11;
	 self.calc->T12 = (self.m_speed - self.m_speed0 - (self.m_accel * self.m_accel)/self.m_jerk)/self.m_accel;
	 if(self.calc->T12 < 0)
	 {
		 self.calc->T12 = 0;
	 }

	 self.calc->T11_s = (self.m_speed0 + (self.m_accel * self.m_accel)/(6*self.m_jerk))*(self.m_accel/self.m_jerk);
	 self.calc->T11_v =  self.m_speed0 + (self.m_accel * self.m_accel)/(2*self.m_jerk);

	 self.calc->T12_s =  self.calc->T11_v * self.calc->T12 + self.m_accel * self.calc->T12 * self.calc->T12 / 2;
	 self.calc->T12_v =  self.m_speed - (self.m_accel * self.m_accel) /(2*self.m_jerk);

	 self.calc->T13_s =  (self.calc->T12_v + (self.m_accel * self.m_accel)/(3*self.m_jerk))*(self.m_accel/self.m_jerk);
	 self.calc->T13_v =  self.calc->T12_v + self.m_accel *  self.calc->T13 - self.m_jerk *  self.calc->T13 *  self.calc->T13 /2;

	 self.calc->T1_s = self.calc->T11_s + self.calc->T12_s + self.calc->T13_s;

	 if(self.calc->T1_s > self.half_dist)
	 {
		 // Distance overshoot, linear period needs to be shorter
		 self.calc_dist = self.half_dist - self.calc->T11_s;

		 self.calc->T12 = solve_quad_positive(
				 	 	 	 self.m_accel/2,
				 	 	  	 self.calc->T11_v+ self.m_accel*self.calc->T11,
							 self.calc->T11* self.calc->T11_v+self.m_accel*self.calc->T11*self.calc->T11/2-self.m_jerk*self.calc->T11*self.calc->T11*self.calc->T11/6-self.calc_dist);


		 self.calc->T12_s =  self.calc->T11_v * self.calc->T12 + self.m_accel * self.calc->T12 * self.calc->T12 / 2;
		 self.calc->T12_v =  self.calc->T11_v + self.m_accel * self.calc->T12;

		 self.calc->T13_s =  (self.calc->T12_v + (self.m_accel * self.m_accel)/(3*self.m_jerk))*(self.m_accel/self.m_jerk);
		 self.calc->T13_v =  self.calc->T12_v + self.m_accel *  self.calc->T13 - self.m_jerk *  self.calc->T13 *  self.calc->T13 /2;

		 self.calc->T1_s  = self.calc->T11_s + self.calc->T12_s + self.calc->T13_s;

	 }

	 if(self.calc->T13_v > self.m_speed)
	 {
		 // Too big top speed - even if distance is ok
		 self.calc->T12 = (self.m_speed  -  self.calc->T11_v + self.m_jerk * self.calc->T13 *self.calc->T13 /2 - self.m_accel * self.calc->T13)/  self.m_accel;

		 if(self.calc->T12 < 0)
		 {
			 self.calc->T12 = 0;
		 }

		 self.calc->T12_s =  self.calc->T11_v * self.calc->T12 + self.m_accel * self.calc->T12 * self.calc->T12 / 2;
		 self.calc->T12_v =  self.m_speed - (self.m_accel * self.m_accel) /(2*self.m_jerk);

		 self.calc->T13_s =  (self.calc->T12_v + (self.m_accel * self.m_accel)/(3*self.m_jerk))*(self.m_accel/self.m_jerk);
		 self.calc->T13_v =  self.calc->T12_v + self.m_accel *  self.calc->T13 - self.m_jerk *  self.calc->T13 *  self.calc->T13 /2;

		 self.calc->T1_s  = self.calc->T11_s + self.calc->T12_s + self.calc->T13_s;

	 }

	 self.calc->T1 = self.calc->T11 + self.calc->T12 + self.calc->T13;
	 self.calc->T2 = 2* (self.half_dist - self.calc->T1_s)/ self.m_speed;
	 self.calc->T2_s = self.calc->T2 * self.m_speed;

	 // These may be different then specified
	 self.calc->accel 		= self.m_accel;
	 self.calc->speed 		= self.m_speed;
	 self.calc->speed_start = self.m_speed0;
	 self.calc->jerk  		= self.m_jerk;



}

int32_t motion_scurve_test_all(void)
{
	int32_t ii;
	int32_t result = 0;

	for(ii =0; ii< DIM(test_cases);ii++)
	{
		result = motion_scurve_test(ii);
		if(result<0)break;
	}

	return result;
}

int32_t motion_scurve_test(uint32_t test_nr)
{
	const test_case_t    * 	tc;
	motion_calc_t    	   	calc;

	int32_t 			   	dist_001mm;
	uint32_t 				speed_safe_001mm_s;
	uint32_t 				speed_001mm_s;
	uint32_t 				accel_001mm_s2;
	uint32_t 				jerk_001mm_s3;
	float					delta = 0.01;


	if(test_nr > DIM(test_cases))
	{
		return -1;
	}

	tc= &test_cases[test_nr];


	dist_001mm			= tc->ts.distance * 1000;
	speed_safe_001mm_s  = tc->ts.speed0   * 1000;
	speed_001mm_s		= tc->ts.speed    * 1000;
	accel_001mm_s2		= tc->ts.accel    * 1000;
	jerk_001mm_s3		= tc->ts.jerk     * 1000;

	motion_scurve_calc(&calc,dist_001mm,speed_safe_001mm_s,speed_001mm_s,accel_001mm_s2,jerk_001mm_s3);


	if(fabs(calc.T11-tc->tr.T11) > delta )
	{
		return -1;
	}

	if(fabs(calc.T12-tc->tr.T12) > delta )
	{
		return -2;
	}

	if(fabs(calc.T13-tc->tr.T13) > delta )
	{
		return -3;
	}

	if(fabs(calc.T2-tc->tr.T2) > delta )
	{
		return -4;
	}

	if(fabs(calc.speed_start - tc->tr.speed0) > delta )
	{
		return -5;
	}

	if(fabs(calc.speed - tc->tr.speed) > delta )
	{
		return -6;
	}

	if(fabs(calc.accel - tc->tr.accel) > delta )
	{
		return -7;
	}

	if(fabs(calc.jerk - tc->tr.jerk) > delta )
	{
		return -8;
	}

	motion_engine_test(dist_001mm,&calc);


	return 0;
}

















