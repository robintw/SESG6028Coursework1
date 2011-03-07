#include<stdio.h>
#include<math.h>
#include <string.h>
#include<stdlib.h>
#include<timer.h>
#include<omp.h>

/* The constant M_PI isn't included in the C89 standard, so we define it here - checking first
to see if it has already been defined (in case the code is compiled with a different C standard */
#ifndef M_PI
#    define M_PI 3.1415926535897931
#endif


/* We define these named constants to make it easier to state whether we want timing info
printed from the integrate() function.*/
#define PRINT_TIMES 1
#define NO_PRINT_TIMES 0

/* Calculate the first function given on the specification sheet */
double f1(double x)
{
	double res;
	
	res = 5*pow(x, 3) + 4 * pow(x, 2) + 3*x + 2;
	
	return res;
}

/* Calculate the second function given on the specification sheet */
double f2(double x)
{
	double res;
	
	res = sin(x);
	
	return res;
}

/* Calculate the third function given on the specification sheet */
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
double integrate( double (*fn)(double), double a, double b, int n, int t, int print_times)
{	
	/* Declare variables
	(All are initialised to zero as C variables will otherwise be initialised to
	whatever happens to be in that memory location) */
	
	double tstart = 0; /* The starting time */
	double tend = 0; /* The ending time */
	
	double h = 0; /* The x-step value */
	double curr_x = 0; /* The current x value used in the loop */
	double sum = 0; /* Used to hold the sum as we go from i = a to b in steps of h */
	int i; /* The integer loop variable */
	
	/* Record the start time */
	tstart = timer();
	
	/* Calculate h (the distance between each x value) given n (the number of bits to split the
	input range into*/
	h = (b - a) / (double) n;


	/* Start a parallel region using the number of threads set by the parameter t
	The region will only be run in parallel if n is greater than 1300
	The loop variable and curr_x variable are private to each thread, all others are shared */
	#pragma omp parallel num_threads(t), default(none), private(i, curr_x), shared(h, a, b, fn, n, sum) if(n>1300)
	
	/* Go from a to b in steps of h and stop when we get to b
	We do this using a standard integer loop, and then calculating what x value
	we are at inside the loop. This means that the loop variable is an int, which is
	more efficient. */
	
	/* Parallelize this loop - splitting the iterations between threads
	Each thread will calculate its own sum, at the end add all of these sub-sums together and store
	it in the variable sum.
	Split the iterations equally between the threads, and do all of the splitting at the beginning. */
	#pragma omp for reduction(+:sum) schedule(static)
	for (i = 0; i < n; i++)
	{
		curr_x = a + i * h;
		/* Calculate the equation given on the Coursework 1 instruction sheet */
		sum += h * ( ( (*fn)(curr_x) + (*fn)(curr_x - h) ) / 2);
	}
	
	/* Record the end time */
	tend = timer();
	
	/* Print the value of n (number of steps), t (number of threads), and the time taken in CSV
	format ready to be redirected to a CSV file from the command line */
	if (print_times == PRINT_TIMES)
	{
		printf("%d, %d, %f\n", n, t, tend-tstart);
	}
	
	/* Return the final calculated value of the integral */
	return sum;
}

double adaptive_integrate(double (*fn)(double), double a, double b, int start_n, double epsilon, int n_factor, int t)
{
	double prev_res = 0;
	double curr_res = 0;
	double diff = 10000;
	int n = start_n;
	double tstart = 0;
	double tend = 0;
	
	/* Record the start time */
	tstart = timer();
	
	while (diff > epsilon)
	{
		prev_res = curr_res;
		curr_res = integrate((*fn), a, b, n, t, NO_PRINT_TIMES);
		printf("Integrating with n = %d\n\tResult = %f\n", n, curr_res);
		n = n * n_factor;
		
		diff = fabs(prev_res - curr_res);
		
		printf("\tDifference between previous results = %f\n", diff);
	}
	
	printf("Difference is less than epsilon. Stopping.\n");
	printf("Final result = %f\n", curr_res);
	
	/* Record the end time */
	tend = timer();
	
	printf("Time taken = %f seconds\n", tend-tstart);
	
	return curr_res;
}

void run_adaptive_test(void)
{
	adaptive_integrate((double (*)(double))f1, -1, 3, 10, 0.0001, 10, 8);
}

void run_all(void)
{
	printf("Using n = 1000\n");
	printf("Integral of f1 = %f\n", integrate((double (*)(double))f1, -1, 3, 10000, 8, NO_PRINT_TIMES));
	printf("Integral of f2 = %f\n", integrate((double (*)(double))f2, 0, 2 * M_PI, 10000, 8, NO_PRINT_TIMES));
	printf("Integral of f3 = %f\n", integrate((double (*)(double))f3, -1 * M_PI, M_PI, 10000, 8, NO_PRINT_TIMES));
}


void run_n_test(void)
{
	double res;
	
	int i;
	
	for (i = 0; i <= 50000; i+=100)
	{
		res = integrate((double (*)(double))f3, -1 * M_PI, M_PI, i, 1, PRINT_TIMES);
		res = integrate((double (*)(double))f3, -1 * M_PI, M_PI, i, 8, PRINT_TIMES);
	}
}

void run_scaling_test(void)
{
	int n = 100000; /* Set n to a large number */
	int i;
	for (i = 1; i <= 8; i++)
	{
		integrate((double (*)(double))f1, -1, 3, n, i, PRINT_TIMES);
		integrate((double (*)(double))f2, 0, 2 * M_PI, n, i, PRINT_TIMES);
		integrate((double (*)(double))f3, -1 * M_PI, M_PI, n, i, PRINT_TIMES);
	}
}

void run_unit_test(void)
{
	/* Use a large n to get values as accurate as possible */
	int n = 1000000;
	
	/* Define the variables for the CALCulated values of the integrals
	and the ANALytical values (derived from Mathematica) */
	double calc_res1, calc_res2, calc_res3 = 0;
	double anal_res1, anal_res2, anal_res3 = 0;
	
	/* For each of the functions, integrate it and store the result and put the analytical result
	in a variable too */
	
	calc_res1 = integrate((double (*)(double))f1, -1, 3, n, 8, NO_PRINT_TIMES);
	anal_res1 = 472 / (double) 3;	
	
	calc_res2 = integrate((double (*)(double))f2, 0, 2 * M_PI, n, 8, NO_PRINT_TIMES);
	anal_res2 = 0.0;
	
	calc_res3 = integrate((double (*)(double))f3, -1 * M_PI, M_PI, n, 8, NO_PRINT_TIMES);
	anal_res3 = 2 - (pow(M_PI,2) / 2);
	
	/* Print the results */
	printf("Function 1 difference: %f\n", calc_res1 - anal_res1);
	printf("Function 2 difference: %f\n", calc_res2 - anal_res2);
	printf("Function 3 difference: %f\n", calc_res3 - anal_res3);
}

/* The main function */
int main(int argc, char *argv[])
{
	/* Define a single int loop variable */
	int i;
	
	/* If no command-line arguments have been specified then display the help text below */
	if (argc == 1)
	{
	printf("SESG6028 Integration Coursework by Robin Wilson\n \
	----------------------------------------------- \
	\nNo command-line arguments specified.\n\nThe following options are available:\n \
	full\t\tRun test of all functions given on coursework sheet\n \
	unittest\t\tRuns a simple test to ensure that the functions are giving the right results\n \
	ntest\t\tRun a test to see what value of n should be used as the if threshold\n \
	scalingtest\tRun a test to determine the scaling of the code\n \
	adaptivetest\tRuns a test of the adaptive integration system using Function 1\n");
    }
    

	/* Process command line arguments checking them and running the appropriate function */
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
        else if (strcmp(argv[i], "adaptivetest") == 0)
        {
        	run_adaptive_test();
        }

    }	

	/* Return success regardless */
	return 0;
}