/* File find_factor.c */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  long N = 4993 * 5393;
  long factor = 0;
#pragma omp parallel
#pragma omp single
  for (long f = 2; f <= N; f++) /* Loop generating tasks */
  {
    { /* Check if f is a factor */
      if (N % f == 0)
      { // the remainder is 0, found factor!
        factor = f;
      }
    } 
    if (factor > 0)
      break;
  } /* End of loop generating tasks */
  if (factor > 0)
    printf("Factor: %li\n", factor);
}
