/**
 * Quicksort implementation using OpenMP
 *
 * Run with make (run). Either supply make with NWORKERS and SIZE,
 * otherwise it will use default values which is NWORKERS = 10
 * and SIZE = 1000000.
 *
 * Can also be run through OMP_NUM_THREADS=NWORKERS ./quicksort.out SIZE
 */


#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 1000000 /* Maximum size of array to be sorted. */
#define BREAKPOINT 50 /* Breakpoint when insertion sort is made instead of quicksort. */
void quicksort(int, int);

int size;
int * vector;

int main(int argc, char ** argv)
{
    if(argc > 1)
    {
        size = atoi(argv[1]);
        if(size > MAXSIZE)
        {
            size = MAXSIZE;
        }
    }
    else
    {
        size = MAXSIZE;
    }
    vector = malloc(sizeof(int) * size);
    #pragma omp parallel
    {
        #pragma omp task
        {
            quicksort(0, size);
        }
    }
    #pragma omp taskwait
    free(vector);
    return(EXIT_SUCCESS);
}

void quicksort(int start, int end)
{
    if(end - start < BREAKPOINT)
    {
        /* TODO Insertionsort. */
    }
    else
    {
        /* TODO Quicksort. */
    }
}
