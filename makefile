# This task will be run by default whenever make is run with no arguments
# It will create the default compile of the code, which should be used most of the time
all:
	# This compiles the integration code using OpenMP so it can be run in parallel
	# The arguments are:
	# -W, -Wall, -pedantic		Warn me about everything possible that could be wrong!
	# -std=c89					Compile using C according to the C89 standard (original ANSI C)
	# -lm						Link to the maths library
	# -fopenmp					Link to the OpenMP library and process #pragma omp directives
	# -O3						Optimise as much as possible
	# -o integrate				Create an executable called integrate
	# -I.						Search for include files in the current directory (used to get the timer.h file)
	gcc -W -Wall -pedantic -std=c89 -lm -fopenmp -O3 -g -o integrate -I. timer.c integrate.c

	# Same as above, but without OpenMP so it will produce purely serial code.
	# NB: This will produce some compiler warnings reminding you that the #pragma omp directives
	# are being ignored
	gcc -W -Wall -pedantic -std=c89 -lm -O3 -o serial_integrate -I. timer.c integrate.c

# Very safe compile of the code, turning off any optimisation and adding debugging info
safe:
	# As above for the parallel version, including:
	# -g						Adds debugging information to the executable
	# NB: No -O flag so no optimisation by the compiler
	gcc -W -Wall -pedantic -std=c89 -lm -fopenmp -g -o integrate -I. timer.c integrate.c
	
	# As above for the serial version, with alterations as above
	gcc -W -Wall -pedantic -std=c89 -lm -g -o serial_integrate -I. timer.c integrate.c
	
test: all
	@echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	@echo Unit Tests
	@echo XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	@echo Parallel version
	@echo ----------------
	@./integrate unittest
	@echo Serial version
	@echo --------------
	@./serial_integrate unittest