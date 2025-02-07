/*!
    \file motion_scurve.c

    \brief
*/


#include "system.h"
#include "math.h"
#include "motion_eq.h"




//
//  https://stackoverflow.com/questions/13328676/c-solving-cubic-equations
//
int32_t  cubic_eq(double a, double b, double c, double d, double * results_re)
{
	double disc, q, r, dum1, s, t, term1, r13;

    if (a == 0)
    {
        return quad_eq(b,c,d,results_re);
    }

    if (d == 0)
    {
        return quad_eq(a,b,c,results_re);
    }

    b   /= a;
    c   /= a;
    d   /= a;

    q    = (3.0*c - (b*b))/9.0;
    r    = -(27.0*d) + b*(9.0*c - 2.0*(b*b));
    r   /= 54.0;
    disc = q*q*q + r*r;

    //The first root is always real.
    term1 = (b/3.0);

    if (disc > 0)
    {
    	// one root real, two are complex
        s = r + sqrt(disc);
        s = ((s < 0) ? -pow(-s, (1.0/3.0)) : pow(s, (1.0/3.0)));
        t = r - sqrt(disc);
        t = ((t < 0) ? -pow(-t, (1.0/3.0)) : pow(t, (1.0/3.0)));
        results_re[0] = -term1 + s + t;

        /*
        term1 += (s + t)/2.0;
        results_re[1] = results_re[2] = -term1;
        term1 = sqrt(3.0)*(-t + s)/2;
        results_im[1] = term1;
        results_im[2] = -term1;
        */
        return 1;
    }

    // The remaining options are all real
    if (disc == 0)
    {
    	// All roots real, at least two are equal.
        r13 = ((r < 0) ? -pow(-r,(1.0/3.0)) : pow(r,(1.0/3.0)));
        results_re[0]= -term1 + 2.0*r13;
        results_re[1] = results_re[2] = -(r13 + term1);
        return 3;
    }


    // Only option left is that all roots are real and unequal (to get here, q < 0)
    q = -q;
    dum1 = q*q*q;
    dum1 = acos(r/sqrt(dum1));
    r13 = 2.0*sqrt(q);
    results_re[0] = -term1 + r13*cos(dum1/3.0);
    results_re[1] = -term1 + r13*cos((dum1 + 2.0*M_PI)/3.0);
    results_re[2] = -term1 + r13*cos((dum1 + 4.0*M_PI)/3.0);

    return 3;
}


int32_t quad_eq(double a, double b, double c,double *results)
{
	double delta,p_delta;

	delta = b*b - 4.0 *a *c;

	if( (delta < 0) || (a==0))
	{
		return 0;
	}

	p_delta = sqrt(delta);

	results[0] = (-b-p_delta)/(2.0*a);
	results[1] = (-b+p_delta)/(2.0*a);


	return 2;
}


