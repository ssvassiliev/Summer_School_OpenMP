---
title:  "Introduction"
teaching: 20
exercises: 0
questions:
- "What is shared-memory programming?"
- "What is OpenMP?"
objectives:
- "Understand the shared-memory programming model and OpenMP standard"
keypoints:
- "OpenMP programs are limited to a single physical machine"
- "You use OpenMP with C, C++, or Fortran"
---

Parallel programs come in two broad flavors: shared-memory and message-passing. In this workshop, we will be looking at shared-memory programming, with a focus on Open Multi- Processing (OpenMP) programming.

## What is shared-memory programming?
In any parallel program, the general idea is to have multiple threads of execution so that you can break up your problem and have each thread handle one part. These multiple threads need to be able to communicate with each other as your program runs. In a shared-memory program, this communication happens through the use of global variables stored in the global memory of the local machine. This means that communication between the various threads is extremely fast, as it happens at the speed of RAM access. But your program will be limited to a single physical machine, since all threads need to be able to see the same RAM.

OpenMP is one way of writing shared-memory parallel programs. OpenMP is actually a specification, which has been implemented by many vendors.

The OpenMP effort began in 1996 when Accelerated
Strategic Computing Initiative of the DOE brought together a handful of computer vendors including HP, IBM, Intel, SGI and DEC to create a portable API for shared memory computers. Vendors do not typically work well together unless an outside force encourages cooperation. So this committee communicated that DOE would only purchase systems with a portable API for shared memory programming.

The current OpenMP v.5.0 specification is 600+ pages long, but you need to know only a very small fraction of it to be able to use it in your code.

[OpenMP specifications](https://www.openmp.org/specifications/)

OpenMP standard describes extensions to a C/C++ or FORTRAN compiler. This means that you need use a compiler that supports OpenMP. There are different OpenMP implementations (compilers) and they may not support some sections of OpenMP specifications. It is up to the programmer to investigate the compiler they want to use and see if it supports the parts of OpenMP that they wish to use. Luckily, the vast majority of OpenMP behaves the way you expect it to with most modern compilers. When possible, we will try and highlight any odd behaviors.

[Compilers supporting OpenMP](https://www.openmp.org/resources/openmp-compilers-tools/)

For an overview of the past, present and future of the OpenMP read the paper ["The Ongoing Evolution of OpenMP"](https://ieeexplore.ieee.org/document/8434208).


## OpenMP Execution Model

- OpenMP programs realizes parallelism through the use of threads. Recollect that a thread is the smallest unit of computing that can be scheduled by an operating system. In other words a thread is a subroutine that can be scheduled to run autonomously. Threads exist within the resources of a single process. Without the process, they cease to exist.

- OpenMP offers the programmer full control over parallelization. It is an explicit, not automatic programming model.

- OpenMP uses the so-called fork-join model of parallel execution:
  - OpenMP programs start as a single process, the master thread. The master thread executes sequentially. When the first parallel region is encountered. The master thread creates a team of parallel threads.We call this “forking”.
  - The statements in the parallel region of the program that are executed in parallel by various team threads.
  - When the team threads complete the statements in the parallel region construct, they synchronize and terminate, leaving only the master thread. We call this “joining”.

- OpenMP divides the memory into two types: Global (or shared) memory, and thread-local memory. Every thread can read and write the global memory, but each thread also gets a little slice of memory that can only be read or written by that thread.
![](../fig/OpenMP-execution-model.svg)

Since OpenMP is meant to be used with either C/C++ or FORTRAN, you will need to know how to work with at least one of these languages. This workshop will use C as the language for the examples. As a reminder, a simple hello world program in C would look like the following.

> ## Compiling C code
> ~~~
> #include <stdio.h>
> #include <stdlib.h>
>
> int main(int argc, char **argv) {
>   printf("Hello World\n");
> }
> ~~~
> {: .source}
>
> In order to compile this code, you would need to use the following command:
>
> ~~~
> gcc -o hello hello_world.c
> ~~~
> {: .bash}
>
> This gives you an executable file "hello" that will print out the text "Hello World". You run it with the command:
>
> ~~~
> ./hello
> ~~~
> {: .bash}
> ~~~
> Hello World
> ~~~
> {: .output}
> If you don't specify the output filename with the -o option compiler will use the default output name "a.out" (assembler output).  
{: .callout}

> ## GCC on Compute Canada
>
> The default environment on the general purpose clusters (Beluga, Cedar, Graham) includes a gcc compiler. On Niagara, for instance, the default environment is very minimal and you must load a module explicitly to access any compiler:
>
> ~~~
> $ module load gcc
> $ gcc --version
> gcc (GCC) 7.3.0
> ~~~
> {: .bash}
{: .callout}

### A Very Quick Introduction to C

- The `#include` directive tells the preprocessor to insert the contents of another file into the source code.
- Include directives are typically used to include the C header files for C functions that are defined outsite of the current source file:

  ~~~
  #include <header_file> // Search in a predefined folders
  #include "header_file>" // Search in the current directory
  ~~~
  {: .source}

- Header files contain declarations of functions, variables and macros.

- The `#define` directive declares constant values to be used throughout your code. For example it is a convenient way to change some parameters used throughout the code:
     ~~~
     # define SIZE 1000
     ~~~
     {: .source}



- Curly braces `{ ...  }` are used to group statements into a block of statements.

- Statement terminator is `;`
- For loop syntax:

  ~~~
  for (initialization; condition test; increment or decrement)
  {
         //Statements to be executed repeatedly
  }
  ~~~
  {: .source}

- You can skip any statements from for loop, but semicolons must be retained. For example, you can do initialization before loop:
  ~~~
  i=10;
  for(;i<100; i++)
  ~~~
  {: .source}

-  Skipping all loop statements  will result in an infinite loop: `for(;;)`

- Function definition:

  ~~~
  return_type function_name( parameter list ) {
     body of the function
  }
  ~~~
  {: .source}

  - Example:
    ~~~
    int max(int num1, int num2)
      {
         int result;
         if (num1 > num2)
            result = num1;
         else
            result = num2;
         return result;
      }
    ~~~
    {: .source}


{% include links.md %}
