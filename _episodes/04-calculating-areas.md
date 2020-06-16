---
title: "Numeric Integration - Calculating Areas"
teaching: 15
exercises: 5
questions:
- "How can we calculate integrals?"
objectives:
- "Learn about critical sections"
keypoints:
- "You can use a critical section to control access to a global variable"
---

In this section, we will use the problem of numeric integration, i.e. calculating areas under curves, to look at how to control access to global variables. As our example, let's say we wanted to integrate the sine function from 0 to Pi. This is the same as the area under the first half of a sine curve. The single-threaded version is below.

~~~
/* --- File integrate_sin.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
	int steps = 1e7;
	double delta = M_PI/steps;
	double total = 0.0;
	int i;

	printf("Using %.0e steps\n", (float)steps);
	for (i=0; i<steps; i++) {
		total = total + sin(delta*i) * delta;
	}
	printf("The integral of sine from 0 to Pi is %.12f\n", total);
}
~~~
{: .source}

> ## Compiling with math
> In order to include the math functions, you need to link in the math library. In GCC, you would use the following:
> ~~~
> [user30@login1 ~]$ gcc -o integrate integrate_sin_omp.c -lm
> ~~~
> {: .bash}
{: .callout}

The answer in this case should be 2. It will be off by a small amount because of the limits of computer representations of numbers.

> ## Step size
> How would you change the step size.  What happens if you do?
>
> > ## Solution
> > You can decrease the step size by increasing the `steps` variable.
> > We normally expect this to increase the accuracy of the result.  Does it?
> > Is there a noticeable effect on the run time?
> {: .solution}
{: .challenge}

To see what happens to the time this program takes, we'll use a new tool. Since
we just want to see the total time, we can use the program `time`.

> ## Timing
> You can use the time utility to get the amount of time it takes for a program to run.
> ~~~
> [user30@login1 ~]$ time -p ./integrate
> Using 1e+07 steps
> Total time is 403.428512 ms
> The integral of sine from 0 to Pi is 2.000000000000
> real 0.41
> user 0.40
> sys 0.00
> ~~~
> {: .bash}
> The `real` output is the useful one; this example took 0.005 seconds to run.
> The `user` and `sys` lines describe how much time was spent in "user" code
> and how much in "system" code, a distinction that doesn't interest us today.
{: .callout}

## Parallelizing numerical integration
How would you parallelize this code to get it to run faster?

Obviously, we could add `#pragma parallel for`. But do we make `total` private, or not?

The data dependency on `total` leads to what we call a _race condition_. Since we are
updating a global variable, there is a race between the various threads as to
who can read and then write the value of `total`. Multiple threads could read
the current value, before a working thread can write the result of its addition. So these
reading threads essentially miss out on some additions to the total. This can be
handled by adding a critical section. A critical section only allows one thread
at a time to run some code block.

~~~
/* --- File integrate_sin_omp.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	float time_total;
	int steps = 1e7;
	double delta = M_PI/steps;
	double total = 0.0;
	int i;

	printf("Using %.0e steps\n", (float)steps);
	/* Get start time */
	clock_gettime(CLOCK_MONOTONIC, &ts_start);

#pragma omp parallel for
	for (i=0; i<steps; i++) {
/* pragma omp critical */
		total += sin(delta*i) * delta;
	}
	/* Get end time */
	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1e9 + \
		     (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("Total time is %f ms\n", time_total/1e6);
	printf("The integral of sine from 0 to Pi is %.12f\n", total);
}
~~~
{: .source}

The `critical` directive is a very general construct that lets you ensure a code line is executed exclusively by one thread.

> ## Parallel Performance
> - Compile the program integrate_sin_omp.c and run it on more that one thread. Did you get the correct result?
> - Insert `critical` directive, recompile the code and run it on more that one thread. Try different number of threads. Did you get the corect result now?
> - How execution time with one thread compares to 2 and more threads?
>
> > ## Solution
> > In this particular case using `critical` directive is not a good decision. We have only one line of code, so all threads except one are waiting. This is reflected in a degraded parallel performance compared even to serial version.
>  {: .solution}
{: .challenge}

Computing a sum is a very common operation and OpenMP provides a specific thread-safe mechanism to compute a sum: *Reduction clause*.
The OpenMP reduction clause lets you specify thread-private variables that are subject to a reduction operation at the end of the parallel region.

> ## Reduction Clause
> - Delele the `critical` directive and add reduction clause to the parallel `for` loop.  Recompile the code and execute. Try different number of threads. Did you get the corect result now?
> - How does the program scale now?
> Hint: specify the variable `total` as a reduction variable: `reduction(+:total)`
>
> > ## Solution
> > `#pragma omp parallel for reduction(+:total)`
>  {: .solution}
{: .challenge}

We'll look at other uses of reduction variables in the next section.
