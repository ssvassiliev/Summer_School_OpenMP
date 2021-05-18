---
title: "Tasks"
teaching: 20
exercises: 5
questions:
- "How to recurse in parallel"
objectives:
- "Use general parallel tasks"
keypoints:
- "OpenMP can manage general parallel tasks"
---

The last technique we will look at is the `task` construct. Tasks is the most recent addition to omp constructs.
Older constructs worked well for many cases but there were limitations. Only loops with a known length at run time were allowed. And only finite number of parallel sections.

This didn’t work well with certain common problems. Linked lists and recursive algorithms for example. Another concept that was hard to parallelize was the `while` loop with it's unknown number of iterations.

The task construct addresses these issues by generating a pool of tasks that are then executed by all available threads. Typically, one thread will generate the tasks, adding them to a queue, from which all threads can take and execute them. This leads to the following code:

~~~
#pragma omp parallel
#pragma omp single
{
...
#pragma omp task
{ ... }
...
#pragma omp task
{ ... }
...
}
~~~
{:.language-c}

A parallel region creates a team of threads; a single thread then creates the tasks, adding them to a queue that belongs to the team, and all the threads in that team execute tasks from this queue. Tasks could run in any order.

If you need to have results from the tasks before you can continue, you can use the `taskwait` directive to tell OpenMP to pause your program until the tasks are done.

> ## Finding the Smallest Factor
> Use tasks to find the smallest factor of a large number (using 4993*5393 as test case). Generate a task for each trial factor. Start with this code:
>
> ~~~
> /* File find_factor.c */
> #include <stdio.h>
> #include <stdlib.h>
> #include <time.h>
> int main()
> {
> long N = 4993*5393;
> long factor=0;
> #pragma omp parallel
> #pragma omp single
> for (long f=2; f <= N; f++)  
>   { /* see if `f' is a factor */
>     if (N%f == 0) {
>       factor = f;
>       printf("Found a factor: %li\n",factor);
>       exit(0);
>     }
>   }
> }
> ~~~
> {:.language-c}
>
> - Turn the factor finding block into a task.
> - Run your program a number of times. Does it find the wrong factor? Why? Try to fix this.
> - Once a factor has been found, you should stop generating tasks.
>
> > ## Solution
> > ~~~
> > #include <stdio.h>
> > #include <stdlib.h>
> > #include <time.h>
> > #include <omp.h>
> >
> > int main()
> > {
> > long N = 4993*3001;
> > long factor=0;
> > #pragma omp parallel
> > #pragma omp single
> > for (long f=2; f<=N; f++)
> >  if(!factor)
> >  #pragma omp task
> >   { /* see if `f' is a factor */
> >    if (N%f == 0) {
> >    factor = f;
> >    printf("Found a factor: %li\n",factor);
> >     }
> >   }
> >}
> > ~~~
> > {:.language-c}
> {: .solution}
{: .challenge}

### Task Synchronization

In general it is impossible to say when a task will be executed. Consider the code:

~~~
x = f();
#pragma omp task
{ y1 = g1(x); }
#pragma omp task
{ y2 = g2(x); }
z = h(y1)+h(y2);
~~~
{:.language-c}

- The code is incorrect, `z = h(y1)+h(y2)` could be executed before y1 and y2 are computed.
- Wait until tasks are finished: `taskwait` directive.

Correct code:
~~~
x = f();
#pragma omp task
{ y1 = g1(x); }
#pragma omp task
{ y2 = g2(x); }
#pragma omp taskwait
z = h(y1)+h(y2);
~~~
{:.language-c}

### Task Dependencies

Consider the following code:
~~~
#pragma omp task
x = f()
#pragma omp task
y = g(x)
~~~~
{:.language-c}

The second task can be executed before the first, possibly leading to an incorrect result. This is corrected by specifying task dependency:

~~~
#pragma omp task depend(out:x)
x = f()
#pragma omp task depend(in:x)
y = g(x)
~~~
{:.language-c}

This example illustrates Read after Write (RaW) dependency: the first task writes x which is then read by the second task.

Since either task can read or write a variable, there are four types of dependencies.

1. `RaW (Read after Write)`. The second task reads an item that the previous task writes. The second task has to be executed after the first:

2. `WaR (Write after Read)`. The first task reads and item, and the second task overwrites it. The second task has to be executed second to prevent overwriting the initial value:

3. `WaW (Write after Write)`. Both tasks set the same variable. Since the variable can be used by an intermediate task, the two writes have to be executed in this order.

4. `RaR (Read after Read)`. Both tasks read a variable. Since neither tasks has an `out' declaration, they can run in either order.

The depend clause enforces additional constraints on the scheduling of tasks or loop iterations. These constraints establish dependences only between sibling tasks or between loop iterations. The task dependence is fulfilled when the predecessor task has completed.


> ## Controlling Order of Task Execution
> Consider the following loop:
>
> ~~~
> for(i=1;i<N;i++)
>   for(j=1;j<N;j++)
>     x[i][j] = x[i-1][j] + x[i][j-1];
> ~~~
> {:.language-c}
>
> The loop cannot be parallelized with `parallel for` construct due to data dependency. The next task should be executed after the previous task updates the variable *x*.
- Use tasks with dependencies to parallelize this code.  
- Start with the incorrectly parallelized code:
>
> ~~~
> /* --- File task_depend.c --- */
> #include <stdlib.h>
> #include <stdio.h>
>
> int main(int argc, char **argv) {
>
>   int N = 8;
>   int x[N][N];
>   int i,j;
>
>   /* Initialize x */
>   for(i=0;i<N;i++)
>     for(j=0;j<N;j++)
>       x[i][j]=i+j;
>
>   /* Serial computation */
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++)
>       x[i][j] = x[i-1][j] + x[i][j-1];
>   }
>
>   printf("Serial result:\n");
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++)
>       printf("%8d ",x[i][j]);
>     printf("\n");
>   }
>
>   /* Reset x */
>   for(i=0;i<N;i++)
>     for(j=0;j<N;j++)
>       x[i][j]=i+j;
>
>   /* Parallel computation */
> #pragma omp parallel 
> #pragma omp single
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++)
>     #pragma omp task
>       x[i][j] = x[i-1][j] + x[i][j-1];
>   }
>
>   printf("Parallel result:\n");
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++)
>       printf("%8d ",x[i][j]);
>     printf("\n");
>   }
> }
> ~~~
> {:.language-c}
> You should be able to do this by only adding OpenMP directives.
> > ## solution
> > ~~~
> > /* --- File task_depend_omp.c --- */
> > #include <stdlib.h>
> > #include <stdio.h>
> >
> > int main(int argc, char **argv) {
> >
> >   int N = 8;
> >   int x[N][N];
> >   int i,j;
> >
> >   /* Initialize x */
> >   for(i=0;i<N;i++)
> >     for(j=0;j<N;j++)
> >       x[i][j]=i+j;
> >
> >   /* Serial computation */
> >   for(i=1;i<N;i++){
> >     for(j=1;j<N;j++)
> >       x[i][j] = x[i-1][j] + x[i][j-1];
> >   }
> >
> >   printf("Serial result:\n");
> >   for(i=1;i<N;i++){
> >     for(j=1;j<N;j++)
> >       printf("%8d ",x[i][j]);
> >     printf("\n");
> >   }
> >
> >   /* Reset x */
> >   for(i=0;i<N;i++)
> >     for(j=0;j<N;j++)
> >       x[i][j]=i+j;
> >
> >   /* Parallel computation */
> > #pragma omp parallel
> > #pragma omp single
> >   /* Generate parallel tasks */
> >   for(i=1;i<N;i++){
> >     for(j=1;j<N;j++)
> > #pragma omp task depend(out:x)
> >       x[i][j] = x[i-1][j] + x[i][j-1];
> >   }
> >
> >   printf("Parallel result:\n");
> >   for(i=1;i<N;i++){
> >     for(j=1;j<N;j++)
> >       printf("%8d ",x[i][j]);
> >     printf("\n");
> >   }
> > }
> > ~~~
> > {:.language-c}
> {: .solution}
{: .challenge}

>## Computing Fibonacci Numbers
>The next example shows how  `task` and `taskwait` directives can be used to compute Fibonacci numbers recursively.
>
>- In the `parallel` construct, the `single` directive calls *fib(n)*.  
>- The call to *fib(n)* generates two tasks. One of the tasks computes *fib(n-1)* and the other computes *fib(n-2)*. 
>- The two return values are added together to produce the value returned by *fib(n)*. 
>- Each of the calls to *fib(n-1)* and *fib(n-2)* will in turn generate two tasks. Tasks will be recursively generated until the argument passed to *fib()* is less than 2.
>
>The `taskwait` directive ensures that both tasks generated in *fib( )* compute *i* and *j* before return.
>
>~~~
>/* --- File fib.c ---*/
>#include <stdio.h>
>#include <stdlib.h>
>#include <omp.h>
>
>/* Usage: ./fib [n] */
>/* Default n=10 */
>
>int fib(int n) {
>   int i, j;
>   if (n<2)
>      return n;
>   else {
>      #pragma omp task shared(i)
>      i=fib(n-1);
>      #pragma omp task shared(j)
>      j=fib(n-2);
>      #pragma omp taskwait
>      return i+j;
>   }
>}
>
>int main(int argc, char **argv){
>   int n=10, result;
>   if(argc==2) {
>       char *a = argv[1];
>       n = atoi(a);
>   }
>
>   #pragma omp parallel
>   {
>      #pragma omp single
>      result = fib(n);
>   }
>   printf("Fibonacci number %d is %d\n", n, result);
>}
>~~~
>{:.language-c}
{:.callout}

> ## Tasks?
> How do tasks work with different number of threads?
>> ## Solution
>>Each task region will be executed by one thread. OpenMP will not use more threads to execute a single task. 
>>
>{:.solution}
{: .challenge}


> ## Task gotchas
> There are a few gotchas to be aware of. While the intention is that tasks will run in parallel, there is nothing in the specification that guarantees this behavior. You may need to check how your particular environment works.
{: .callout}

