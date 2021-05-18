---
title: "Parallel Operations with Arrays"
teaching: 20
exercises: 20
questions:
- "How do I parallelize a loop?"
objectives:
- "Use the PARALLEL FOR pragma (or PARALLEL DO)"
- "Use the PRIVATE clause"
- "Be able to identify data dependencies"
keypoints:
- "The PARALLEL FOR (or PARALLEL DO) pragma makes a loop execute in parallel"
- "A single variable accessed by different threads can cause wrong results"
- "The PRIVATE clause makes a copy of a variable for each thread"
---

One of the classic applications of programming is linear algebra, in all of its beauty and complexity. We will use a few simple problems to see how to execute loops in parallel using OpenMP.

## Multiply an array by a constant

The simplest problem is applying some function to an array of numbers. An example is multiplying each value by some constant number. In serial you would use a for loop (in C; a DO loop in Fortran) to do this multiplication for each element, like this:

~~~
/* --- File array_multiply.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 1e8;
	int multiplier = 2;
	int *a, *c;
	int i;
	float time_total;

	/* Allocate memory for arrays */
	a = malloc(size*sizeof(int));
	c = malloc(size*sizeof(int));

	/* Get start time */
	clock_gettime(CLOCK_MONOTONIC, &ts_start);

	for (i = 0; i<size; i++) {
		c[i] = multiplier * a[i];
	}

	/* Get end time */
	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1e9 + \
		     (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("Total time is %f ms\n", time_total/1e6);
}
~~~
{: .language-c}

> ## Compiling and running
> Compiling: 
>~~~
gcc array_multiply.c -o array_multiply
>~~~
>{:.language-bash}
> Running with 4 threads:
>~~~
> srun --mem-per-cpu=4000 array_multiply
>~~~
>{:.language-bash}
{: .callout}

We added calls to *clock_gettime( )* from the *time.h* header file to get the start and end times of the heavy work being done by the for loop. In this case, we get a count of how many seconds and how many nanoseconds elapsed, given in two parts of the time structure. We did some math to get the elapsed time in milliseconds.

Try to run the program several times and record execution time. What happens to the run time of your program through multiple runs?

Then change the size of the array, recompile and rerun the program. Did execution time changed?


> ## Time and Size
> What happens to the run time of your program through multiple runs?
> What happens if you change the size and recompile and rerun?
{: .challenge}

How complicated is it to turn this program into a parallel program?  
Not complicated at all.  We add two lines:
~~~
#include <omp.h> 
~~~
{: .language-c}

and  
~~~
#pragma omp parallel for
~~~
{: .language-c}

just before the main `for` loop:

~~~
/* --- File array_multiply_omp.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 1e8;
	int multiplier = 2;
	int *a, *c;
	int i;
	float time_total;

	/* Allocate memory for arrays */
	a = malloc(size*sizeof(int));
	c = malloc(size*sizeof(int));

	/* Get start time */
	clock_gettime(CLOCK_MONOTONIC, &ts_start);

#pragma omp parallel for
	for (i = 0; i<size; i++) {
		c[i] = multiplier * a[i];
	}
	/* Get end time */
	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1e9 + \
		     (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("Total time is %f ms\n", time_total/1e6);
}
~~~
{: .language-c}

> ## Compiling and running
>
> Compiling:
>~~~
>gcc array_multiply_omp.c -o array_multiply_omp -fopenmp
>~~~
>{:.language-bash}
>
> Running with 4 threads:
>~~~
> srun -c4 --mem-per-cpu=1000 array_multiply_omp
>~~~
>{:.language-bash}
{: .callout}

Run the program with different number of threads and observe how the runtime changes.

> ## Using more threads
> How many threads did you use?
> What happens to the runtime when you change the number of threads?
{: .challenge}

In this case, the number of iterations around the for loop gets divided across the number of threads available. In order to do this, however, OpenMP needs to know how many iterations there are in the for loop. It also can't change part way through the loops. To ensure this, there are some rules:
* You must not change the value of *size* within one of the iterations.
* You must not use a call to `break()` or `exit()` within the for loop, either. These functions pop you out of the for loop before it is done.

## Summing the values in a matrix
Now let's try adding up the elements of a matrix.
Moving up to two dimensions adds a new layer of looping. The basic code looks like the following.

~~~
/* --- File matrix_multiply_omp.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 1e4;
	int **a, *c, total;
	int i, j;
	float time_total;

	/* Allocate memory */
	c=malloc(size*sizeof(int));
	a=(int**)malloc(size*sizeof(int*));
	for(i=0;i<size;i++)
		a[i]=malloc(size*sizeof(int));

	/* Set the matrix values to 1 */
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			a[i][j] = 1;
		}
	}

	/* Zero the accumulator */
	for (i=0; i<size; i++) {
		c[i] = 0;
	}
	total = 0;

	clock_gettime(CLOCK_MONOTONIC, &ts_start);

#pragma omp parallel for
	for (i = 0; i<size; i++) {
		for (j=0; j<size; j++) {
			c[i] += a[i][j];
		}
	}

	for (i=0; i<size; i++) {
		total += c[i];
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1e9 + \
		     (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("Total is %d, time is %f ms\n", total, time_total/1e6);
}
~~~
{: .language-c}

> ## Is the result correct?
> What should be the result of this code?
> Is that what it does?  If not, what might be wrong?
> Does it do the same for different number of used CPUs?
>
> > ## Solution
> > The elements all have value 1, and there are 1e4*1e4 of them, so the total should be 1e8. Why isn't it?
> >
> > Remember that OpenMP threads *share memory*. This means that every thread
> > can see and access all of memory for the process. In the above case, multiple threads are all accessing the global variable `j` at the same time.
> >
> > OpenMP includes a method to manage this correctly with the addition of a clause `private()`.
> >
> > ~~~
> >    ...
> >    #pragma omp parallel for private(j)
> >    ...
> > ~~~
> > {: .language-c}
> >
> > This makes sure that every thread has their own private copy of `j` to be used for the inner for loop.
> {: .solution}
{: .challenge}

## Data Dependencies

> ## Definition
> If two statements read or write the same memory location, and at least one
> of the statements writes that memory location, then there is a *data dependency* on that memory location between the two statements.
{: .callout}

In turning a serial program into a parallel program, it's vital to maintain the correctness of the serial program.  (Assuming the serial program is correct, of course, but that's a separate question.)  Data dependency is an important concept we use to guarantee that we're transforming a serial program into an equivalent (in terms of its output) parallel program.

_If there is a data dependency between two statements, then the order in which those statements are executed may affect the output of the program, and hence its correctness._

Consider this loop that computes a cumulative sum:
~~~
for ( i=2; i<N; i=i+1 ) {
    a[i] = a[i] + a[i-1];
}
~~~
{: .language-c}

The iteration with `i==2` for example reads locations `a[1]` and `a[2]` and
writes location `a[2]`.  The iteration with `i==3` then reads locations
`a[2]` and `a[3]`.  Since `a[2]` was read by both and written by one, there is
a data dependency on `a[2]` between the assignment statement in successive loop
iterations.   So, the "statements" in the definition don't need to be separate
lines of code.

Now think about what it would mean to run this loop in parallel:  Different
iterations would be carried out by different threads of execution, possibly at the same
time.  If any two iterations didn't happen in the order dictated by the serial
code, the results would be wrong.

There are three types of data dependencies:
* Flow dependencies, like the last example, when one statement uses the results of another
* Anti-dependencies, when one statement should write to a location only ''after'' another has read what's there
* Output dependencies, when two statements write to a location, so the result will depend on which one wrote to it last
There's a wikipedia page on data dependencies: <https://en.wikipedia.org/wiki/Data_dependency>

> ## Is There a Dependency?
>
> Which of the following loops have data dependencies?
>
> ~~~
> /* loop #1 */
> for ( i=2; i<N; i=i+2 ) {
>     a[i] = a[i] + a[i-1]; }
>
> /* loop #2 */
> for ( i=1; i<N/2; i=i+1 ) {
>     a[i] = a[i] + a[i+N/2]; }
>
> /* loop #3 */
> for ( i=0; i<N/2+1; i=i+1 ) {
>     a[i] = a[i] + a[i+N/2]; }
>
> /* loop #4 */
> for ( i=1; i<N; i=i+1 ) {
>     a[idx[i]] = a[idx[i]] + b[idx[i]]; }
> Depends
> ~~~
> {: .language-c}
>
> > ## Solution
> >
> > Loop #1 does not.
> > The increment of this loop is 2, so in the step a[2]=a[2]+a[1]. In the next iteration we compute a[4]=a[4]+a[3] ... etc.
> >
> > Loop #2 does not.
> > In this range of i values each thread modifies only one element of array a.
> >
> > Loop #3 does.
> > Here the last iteration creates data dependency writing to a[N/2]:
> >
> > a[N/2] = a[N/2] + a[N]
> >
> > a[0] = a[0] + a[N/2]
> >
> > Loop #4 might or might not, depending on the contents of array `idx`.
> > If any two entries of `idx` are the same, then there's a dependency.
> >
> {: .solution}
{: .challenge}

## Thread-safe functions

Another important concept is that of a *thread-safe* function.  A thread-safe function is one which can be called simultaneously by multiple
threads of execution.  You need to be aware when writing multi-threaded programs whether functions you are using are thread-safe or not. A classic example of this is when generating pseudo-random numbers. Most random number generators maintain state between calls to them. If they aren't written to be thread-safe, then that internal state can get mixed up by multiple threads calling them at the same time.  For more, see [Thread safety](https://en.wikipedia.org/wiki/Thread_safety).
