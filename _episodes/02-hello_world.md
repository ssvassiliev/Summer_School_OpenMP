---
title: "Hello World"
teaching: 20
exercises: 10
questions:
- How do you compile and run an OpenMP program?
- What are OpenMP pragmas?
- How to identify threads?
objectives:
- Write, compile and run a multi-threaded program where each thread prints “hello world”.
keypoints:
- "Pragmas are directives to the compiler to parallelize something"
- "Thread number is typically controlled with an environment variable, OMP_NUM_THREADS"
- "Order of execution of parallel elements is not guaranteed."
- "If the compiler doesn't recognize OpenMP pragmas, it will compile a single-threaded program.  But you may need to escape OpenMP function calls."
---

## Adding parallelism to a program
Since OpenMP is an extension to the compiler, you need to be able to tell the compiler when and where to add the code necessary to create and use threads for the parallel sections. This is handled through special statements called pragmas. To a compiler that doesn't understand OpenMP, pragmas look like comments. The basic forms are:

C/C++
~~~
#pragma omp ...
~~~
{: .source}

FORTRAN
~~~
!$OMP ...
~~~
{: .source}


How do we add in parallelism to the basic hello world program? The very first pragma that we will look at is the `parallel` pragma.

~~~
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv) {

   #pragma omp parallel
   printf("Hello World\n");
}
~~~
{: .source}

To compile it, you'll need to add an extra flag to tell the compiler to treat the source code as an OpenMP program.

~~~
gcc -fopenmp -o hello hello.c
~~~
{: .source}

If you prefer Intel compilers to GCC, use:

~~~
module load intel/2019.3
icc -qopenmp -o hello hello.c
~~~
{: .source}
**NOTE:** Intel compilers are proprietary, they are available on CC cluster, but not on our test cluster.

When you run this program, you should see the output "Hello World" multiple
times. But how many?

The standard says this is implementation dependent. But the usual default is, OpenMP will look at the machine that it is running on and see how many cores there are. It will then launch a thread for each core.

You can control the number of threads with environment variable OMP_NUM_THREADS. For example, if you want only 3 threads, do the following:

~~~
export OMP_NUM_THREADS=3
./hello
~~~
{: .bash}

> ## Using multiple cores
> Try running the hello world program with different numbers of threads.
> - Can you use more threads than the cores on the machine?
> You can use *nproc* command to find out how many cores are on the machine.
{: .challenge}

> ## OpenMP with Slurm
> When you wish to submit an OpenMP job to the job scheduler Slurm, you can use the following boilerplate.
> ~~~
> #SBATCH --account=sponsor0
> #SBATCH --time=0:01:0
> #SBATCH --cpus-per-task=3
> export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
> ./hello
> ~~~
> {: .bash}
>
> You could also ask for an interactive session with multiple cores like so:
> ~~~
> salloc --account=sponsor0 --cpus-per-task=3 --time=1:0:0
> ~~~
> {: .bash}
> ~~~
> [user30@login1 ~]$ salloc --account=sponsor0 --cpus-per-task=3 --time=1:0:0
> salloc: Granted job allocation 54
> salloc: Waiting for resource configuration
> salloc: Nodes c14r56g1-node1 are ready for job
> [user30@c14r56g1-node1 ~]$ nproc
> 3
> ~~~
> {: .output}
>  The most practical way to run our short parallel program is using *srun* command. Instead of submitting the job to the queue  *srun* will run the program from the interactive shell as soon as requested resources will become available. *Srun* understands the same keywords as *sbatch* and *salloc*.
>
> In SLURM environment operating system will see as many CPUs as you requested, so there is no need to set OMP_NUM_THREADS variable.
>
> ~~~
> srun --cpus-per-task=4 hello
> # or even shorter:
> srun -c4 hello
> ~~~
> {: .bash}
{: .callout}



## Identifying threads

How can you tell which thread is doing what? The OpenMP specification includes a number of functions that are made available through the included header file "omp.h". One of them is the function "omp_get_thread_num()", to get an ID of the thread running the code.

~~~
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv) {
   int id;

   #pragma omp parallel
   {
     id = omp_get_thread_num();
     printf("Hello World from thread %d\n", id);
   }
}
~~~
{: .source}

Here, you will get each thread tagging their output with their unique ID, a number between 0 and NUM_THREADS-1.

> ## Pragmas and code blocks
> An OpenMP pragma applies to the following *code block* in C or C++.
> Code blocks are either a single line, or a series of lines wrapped by curly brackets.
> Because Fortran doesn't have an analogous construction, many OpenMP pragmas in Fortran are paired with an "end" pragma, such as `!$omp parallel end`.
{: .callout}

> ## Thread ordering
> Try running the program a few times.
> - What order do the threads write out their messages in?
> - What's going on?
>
> > ## Solution
> > The messages are emitted in random order. This is an important rule of not only OpenMP programming, but parallel programming in general: parallel elements are scheduled to run by the operating system and order of their execution is not guaranteed.
> {: .solution}
{: .challenge}

> ## Conditional compilation
> We said earlier that you should be able to use the same code for both OpenMP and serial work. Try compiling the code without the -fopenmp flag.
> - What happens?
> - Can you figure out how to fix it?
>
> Hint: The compiler defines preprocessor variable \_OPENMP, so you could use #ifdef ... #endif preprocessor directives
> > ## Solution
> > ~~~
> >
> > #include <stdio.h>
> > #include <stdlib.h>
> > #include <omp.h>
> >
> > int main(int argc, char **argv) {
> >    int id = 0;
> >    #pragma omp parallel
> >    {
> > #ifdef _OPENMP
> >    id = omp_get_thread_num();
> > #endif
> >    printf("Hello World from thread %d\n", id);
> >    }
> > }
> > ~~~
> > {: .source}
> {: .solution}
{: .challenge}

{% include links.md %}
