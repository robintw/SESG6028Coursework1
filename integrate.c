#include<stdio.h>
#include<math.h>

double f1(double x)
{
	double res;
	res = sin(x);
	return res;
}

/* This function does the actual integration
x: an array of x-values of the function
y: an array of the corresponding y-values for each x-value
n: the number of iterations to do
*/
double integrate(double a, double b, int n)
{	
	/* Declare variables
	(All are initialised to zero as C variables will otherwise be initialised to
	whatever happens to be in that memory location) */
	double h = 0; /* The x-step value */
	double curr_x = 0; /* The current x value used in the loop */
	double sum = 0; /* Used to hold the sum as we go from i = a to b in steps of h */
	
	/* Calculate h (the distance between each x value) given n (the number of bits to split the
	input range into*/
	h = (b - a) / (double) n;

	/* Go from a to b in steps of h
	Stop when we get to b */
	for (curr_x = a; curr_x <= b; curr_x += h)
	{
		/* Calculate the equation given on the Coursework 1 instruction sheet
		(This is Simpson's rule) */
		sum += h * ( ( sin(curr_x) + sin(curr_x - h) ) / 2);
	}
	
	return sum;
}

/* The main function */
int main(void)
{	
	printf("%f\n", integrate(0, 1, 1000));

	return 0;
}