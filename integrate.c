#include<stdio.h>
#include<math.h>
#include <string.h>
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
*fn		A pointer to a function to intergrate
a		The lower limit for integration
b		The upper limit for integration
n		The number of trapezia to use for integration
t		The number of threads to use (given as an argument so that tests can easily be carried out)
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

void run_scaling_test(void)
{
	int n = 100000; /* Set n to a large number */
	int i;
	for (i = 1; i <= 8; i++)
	{
		integrate((double (*)(double))f1, -1, 3, n, i);
		integrate((double (*)(double))f2, 0, 2 * M_PI, n, i);
		integrate((double (*)(double))f3, -1 * M_PI, M_PI, n, i);
	}
}

void run_unit_test(void)
{
	int n = 1000000;
	
	double calc_res1, calc_res2, calc_res3 = 0;
	double anal_res1, anal_res2, anal_res3 = 0;
	
	calc_res1 = integrate((double (*)(double))f1, -1, 3, n, 8);
	anal_res1 = 472 / (double) 3;
	
	
	
	calc_res2 = integrate((double (*)(double))f2, 0, 2 * M_PI, n, 8);
	anal_res2 = 0.0;
	
	
	
	calc_res3 = integrate((double (*)(double))f3, -1 * M_PI, M_PI, n, 8);
	anal_res3 = 2 - (pow(M_PI,2) / 2);
	
	printf("Unit test results:\n---------------\n");
	printf("Function 1: %f\n", calc_res1 - anal_res1);
	printf("Function 2: %f\n", calc_res2 - anal_res2);
	printf("Function 3: %f\n", calc_res3 - anal_res3);
}

/* The main function */
int main(int argc, char *argv[])
{
	int i;
	
	if (argc == 1)
	{
	printf("SESG6028 Integration Coursework by Robin Wilson\n \
	----------------------------------------------- \
	\nNo command-line arguments specified.\n\nThe following options are available:\n \
	full\t\tRun test of all functions given on coursework sheet\n \
	ntest\t\tRun a test to see what value of n should be used as the if threshold\n \
	scalingtest\tRun a test to determine the scaling of the code\n");
    }
    

	/* Process command line arguments */
	for (i = 1; i < argc; i++)  /* Skip argv[0] (program name). */
    {
    	/* if strcmp returns 0 then the strings are identical */
        if (strcmp(argv[i], "full") == 0)
        {
            run_all();
        }
        else if (strcmp(argv[i], "ntest") == 0)
        {
            run_n_test();
        }
        else if (strcmp(argv[i], "scalingtest") == 0)
        {
            run_scaling_test();
        }
        else if (strcmp(argv[i], "unittest") == 0)
        {
            run_unit_test();
        }

    }	

	return 0;
}