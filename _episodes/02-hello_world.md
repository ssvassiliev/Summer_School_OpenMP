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
#pragma omp < OpenMP directive >
~~~
{: .source}

FORTRAN
~~~
!$OMP < OpenMP directive >
~~~
{: .source}

In C all OpenMP - specific directives start with `#pragma omp`.

How do we add in parallelism to the basic hello world program? The very first directive that we will look at is the `parallel` directive. The `parallel` directive forks threads to carry out the work given in the `parallel` block of code.

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

The OpenMP standard says this is implementation dependent. But the usual default is that OpenMP will look at the machine it is running on and see how many cores there are. It will then launch a thread for each core.

You can control the number of threads with environment variable OMP_NUM_THREADS. For example, if you want only 3 threads, do the following:

~~~
export OMP_NUM_THREADS=3
./hello
~~~
{: .bash}

> ## Using multiple cores
> Try running the "hello" program with different numbers of threads.
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
>  The most practical way to run our short parallel program on our test cluster is using *srun* command. Instead of submitting the job to the queue  *srun* will run the program from the interactive shell as soon as requested resources will become available. After the job is finished slurm will release the allocated resources and exit. *Srun* understands the same keywords as *sbatch* and *salloc*.
>
> In SLURM environment operating system will see as many CPUs as you requested, so there is no need to set OMP_NUM_THREADS variable to $SLURM_CPUS_PER_TASK.
>
> ~~~
> srun --cpus-per-task=4 hello
> # or even shorter:
> srun -c4 hello
> ~~~
> {: .bash}
{: .callout}

## Identifying threads

> ## Download and Unpack the Code.
> If you have not yet done so, download and unpack the code:
> ~~~
> cd Desktop
> wget
> tar -xf
> cd code
> ~~~
> {: .source}
{: .callout}

How can you tell which thread is doing what? The OpenMP specification includes a number of functions that are made available through the included header file "omp.h". One of them is the function "omp_get_thread_num( )", used to get an ID of the thread running the code.

~~~
/* --- File hello_world_omp.c --- */
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

Here, you will get each thread tagging their output with their unique ID, a number between 0 and (number of threads - 1).

> ## Pragmas and code blocks in FORTRAN
> An OpenMP directive applies to the *code block* following it in C or C++. Code blocks are either a single line, or a series of lines wrapped by curly brackets.
>
> Because Fortran doesn't have an analogous construction, many OpenMP directives in Fortran are paired with the matching "end" directive, such as `!$omp parallel end`.
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

> ## Conditional Compilation
> We said earlier that you should be able to use the same code for both OpenMP and serial work. Try compiling the code without the -fopenmp flag.
> - What happens?
> - Can you figure out how to fix it?
>
> Hint: The compiler defines preprocessor macro \_OPENMP, so you could use #ifdef ... #endif preprocessor directives
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


### Work-Sharing Constructs

A work-sharing construct divides the execution of the enclosed code region among the members of the thread team that encounter it.

- Work-sharing constructs do not launch new threads
- A program will wait for all threads to finish at the end of a work sharing construct. This behaviour is called "implied barrier".

#### *For*
- ***For*** construct divides iterations of a loop across the team of threads.
- Each thread executes the same instructions. This assumes a parallel region has already been initiated, otherwise it executes in serial on a single processor.
- *For* represents a type of *data parallelism*.

~~~
...
#pragma omp parallel for
    for (i=0; i < N; i++)
        c[i] = a[i] + b[i];
...
~~~
{: .source}

> ## Stack Overflow
> If you declare large arrays like this:
> ~~~
>  A[1000][1000];
> ~~~
> Your program may emit "Segmentation fault" message and crash. These arrays are allocated on stack and stack on our cluster is very limited (8MB).
{: .callout}

#### *Sections*
- ***Sections*** construct breaks work into separate, discrete sections.
- It is is a non-iterative work-sharing construct.
- It specifies that the enclosed section(s) of code are to be divided among the threads in the team. Independent *section* directives are nested within a *sections* directive. Each *section* contains different instructions and is executed once by a thread.
- It is possible for a thread to execute more than one *section*.
- *Sections* can be used to implement a *functional parallelism*.

~~~
#pragma omp parallel shared(a,b,c,d) private(i)
  {
#pragma omp sections nowait
    {

#pragma omp section
    for (i=0; i < N; i++)
      c[i] = a[i] + b[i];

#pragma omp section
    for (i=0; i < N; i++)
      d[i] = a[i] * b[i];

    }  /* end of sections */
  }  /* end of parallel region */
~~~
{: .source}

Here *nowait* keyword (clause) means that the program will not wait at the end of `sections` block for all threads to finish.

> ## Exercise
> Compile the file *sections.c* and run it on a different number of CPUs. This example has two sections and the program prints out which threads are doing them.
> - What happens if the number of threads and the number of *sections* are different?
> - More threads than *sections*?
> - Less threads than sections?
>
> > ## Solution
> > If there are more threads than sections, only some threads will execute a section.  If there are more sections than threads, the implementation defines how the extra sections are executed.
> {: .solution}
{: .challenge}

#### *Single*
- The *single* directive specifies that the enclosed code is to be executed by only one thread in the team.
- May be useful when dealing with sections of code that are not thread safe (such as I/O).

{% include links.md %}
