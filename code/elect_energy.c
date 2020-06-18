/* --- File elect_energy.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 20;
	int n_charges = size*size*size;
	float scale=0.5;
	float *charge, *x, *y, *z;
	float **M;
	int i,j,k;
	float time_total;

	charge=malloc(n_charges*sizeof(float));
	x=malloc(n_charges*sizeof(float));
	y=malloc(n_charges*sizeof(float));
	z=malloc(n_charges*sizeof(float));
	M=(float**)malloc(n_charges*sizeof(float*));
	for (i=0;i<n_charges;i++)
		M[i]=malloc(n_charges*sizeof(float));

/* initialize x,y,z coordinates and charges */
	int n=0;
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
			for (k=0; k<size; k++) {
				x[n]=i*scale;
				y[n]=j*scale;
				z[n]=k*scale;
				charge[n]=0.33;
				n++;
			}
	clock_gettime(CLOCK_MONOTONIC, &ts_start);


	// Calculate electrostatic energy: sum of charge[i]*charge[j]/dist[i,j] */
	float dx, dy, dz, dist;
	double Energy=0.0f;

/* Loop calculating  Energy = charge[i]*charge[j]/dist[i,j] */

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1000000000 + (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("\nTotal time is %f ms, Energy is %f\n", time_total/1000000, Energy);
}
