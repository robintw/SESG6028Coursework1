#include<stdio.h>
#include<math.h>
#include<omp.h>

/* The constant M_PI isn't included in the C89 standard, so we define it here - checking first
to see if it has already been defined (in case the code is compiled with a different C standard */
#ifndef M_PI
#    define M_PI 3.1415926535897931
#endif

double f1(double x)
{
	double res;
	
	res = 5*pow(x, 3) + 4 * pow(x, 2) + 3*x + 2;
	
	return res;
}

double f2(double x)
{
	double res;
	
	res = sin(x);
	
	return res;
}

double f3(double x)
{
	double res;
	
	if (x <= 0)
	{
		res = x;
	}
	else
	{
		res = sin(x);
	}
	
	return res;
}

/* This function does the actual integration
x: an array of x-values of the function
y: an array of the corresponding y-values for each x-value
n: the number of iterations to do
*/
double integrate( double (*fn)(double), double a, double b, int n)
{	
	double tstart;
	double tend;
	/* Declare variables
	(All are initialised to zero as C variables will otherwise be initialised to
	whatever happens to be in that memory location) */
	double h = 0; /* The x-step value */
	double curr_x = 0; /* The current x value used in the loop */
	double sum = 0; /* Used to hold the sum as we go from i = a to b in steps of h */
	
	tstart = omp_get_wtime();
	
	/* Calculate h (the distance between each x value) given n (the number of bits to split the
	input range into*/
	h = (b - a) / (double) n;

	/* Go from a to b in steps of h
	Stop when we get to b */
	for (curr_x = a; curr_x <= b; curr_x += h)
	{
		/* Calculate the equation given on the Coursework 1 instruction sheet
		(This is Simpson's rule) */
		sum += h * ( ( (*fn)(curr_x) + (*fn)(curr_x - h) ) / 2);
	}
	
	tend = omp_get_wtime();
	
	printf("Time taken: %f\n", tend-tstart);
	
	return sum;
}

/* The main function */
int main(void)
{	
	printf("Using n = 1000\n");
	printf("Integral of f1 = %f\n", integrate((double (*)(double))f1, -1, 3, 1000));
	printf("Integral of f2 = %f\n", integrate((double (*)(double))f2, 0, 2 * M_PI, 1000));
	printf("Integral of f3 = %f\n", integrate((double (*)(double))f3, -1 * M_PI, M_PI, 1000));

	return 0;
}