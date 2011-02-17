all:
	gcc -W -Wall -pedantic -std=c89 -lm -fopenmp -o integrate integrate.c
