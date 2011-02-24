all:
	gcc -W -Wall -pedantic -std=c89 -lm -fopenmp -o integrate -I. timer.c integrate.c
