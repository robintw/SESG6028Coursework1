#include<stdio.h>
#include<math.h>
#include<timer.h>
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
double integrate( double (*fn)(double), double a, double b, int n, int t)
{	
	double tstart;
	double tend;
	/* Declare variables
	(All are initialised to zero as C variables will otherwise be initialised to
	whatever happens to be in that memory location) */
	double h = 0; /* The x-step value */
	double curr_x = 0; /* The current x value used in the loop */
	double sum = 0; /* Used to hold the sum as we go from i = a to b in steps of h */
	int i;
	
	tstart = timer();
	
	/* Calculate h (the distance between each x value) given n (the number of bits to split the
	input range into*/
	h = (b - a) / (double) n;

	#pragma omp parallel num_threads(t), default(none), private(i, curr_x), shared(h, a, b, fn, n, sum)
	
	/* Go from a to b in steps of h and stop when we get to b
	We do this using a standard integer loop, and then calculating what x value
	we are at inside the loop. This means that the loop variable is an int, which is
	more efficient. */
	
	#pragma omp for reduction(+:sum)
	for (i = 0; i < n; i++)
	{
		curr_x = a + i * h;
		/* Calculate the equation given on the Coursework 1 instruction sheet */
		sum += h * ( ( (*fn)(curr_x) + (*fn)(curr_x - h) ) / 2);
	}
	
	tend = timer();
	
	printf("%d, %d, %f\n", n, t, tend-tstart);
	
	return sum;
}

void run_all(void)
{
	printf("Using n = 1000\n");
	printf("Integral of f1 = %f\n", integrate((double (*)(double))f1, -1, 3, 1000, 8));
	printf("Integral of f2 = %f\n", integrate((double (*)(double))f2, 0, 2 * M_PI, 1000, 8));
	printf("Integral of f3 = %f\n", integrate((double (*)(double))f3, -1 * M_PI, M_PI, 1000, 8));
}

void run_n_test(void)
{
	double res;
	
	int i;
	
	for (i = 0; i <= 5000; i+=10)
	{
		res = integrate((double (*)(double))f3, -1 * M_PI, M_PI, i, 1);
		res = integrate((double (*)(double))f3, -1 * M_PI, M_PI, i, 8);
	}
}

/* The main function */
int main(void)
{	
	run_n_test();	

	

	return 0;
}