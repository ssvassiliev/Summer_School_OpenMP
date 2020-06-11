#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
   struct timespec ts_start, ts_end;
   int size = 1000000;
   int multiplier = 2;
   int a[size];
   int c[size];
   int i;
   float time_total;

   clock_gettime(CLOCK_MONOTONIC, &ts_start);

   for (i = 0; i<size; i++) {
      c[i] = multiplier * a[i];
   }

   clock_gettime(CLOCK_MONOTONIC, &ts_end);
   time_total = (ts_end.tv_sec - ts_start.tv_sec)*1000000000 + (ts_end.tv_nsec - ts_start.tv_nsec);
   printf("Total time is %f ms\n", time_total/1000000);
}
