/* --- File task_depend_omp.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {

    int N = 10;
    int x[N][N];
    int i,j;

    for(i=0;i<N;i++)
	for(j=0;j<N;j++)
	    x[i][j]=i+j;

printf("serial\n");
    for(i=1;i<N;i++){
	for(j=1;j<N;j++){
	    x[i][j] = x[i-1][j] + x[i][j-1];
	    printf("%8d ",x[i][j]);
	}
	printf("\n");
    }


    for(i=0;i<N;i++)
	for(j=0;j<N;j++)
	    x[i][j]=i+j;
    printf("\n");

#pragma omp parallel
#pragma omp single
    for(i=1;i<N;i++)
	for(j=1;j<N;j++)
#pragma omp task depend(out:x)
	    x[i][j] = x[i-1][j] + x[i][j-1];

printf("parallel\n");
    for(i=1;i<N;i++){
	for(j=1;j<N;j++)
	    printf("%8d ",x[i][j]);
	printf("\n");
    }


}
