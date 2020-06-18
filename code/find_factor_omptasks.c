#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main()
{
    long N = 4993*3001;
    long factor=0;
#pragma omp parallel
#pragma omp single
    for (long f=2; f<=N; f++)
	if(!factor)
#pragma omp task
	{ // see if `f' is a factor
	    if (N%f == 0) { // found factor!
		factor = f;
		printf("Found a factor: %li\n",factor);
	    }
	}
}
