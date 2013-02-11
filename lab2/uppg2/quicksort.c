/**
 * Quicksort implementation using OpenMP
 *
 * Run with make (run). Either supply make with NWORKERS and SIZE,
 * otherwise it will use default values which is NWORKERS = 10
 * and SIZE = 1000000.
 *
 * Define DEBUG for a debug prints. DO NOT USE with large SIZE!
 *
 * Can also be run through OMP_NUM_THREADS=NWORKERS ./quicksort.out SIZE
 */


#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAXSIZE 20000000 /* Maximum size of array to be sorted. */
#define BREAKPOINT 50 /* Breakpoint when insertion sort is made instead of quicksort. */
void quicksort(int, int);

int size; /* Size of the array to be sorted. */
int * vector; /* The array to be sorted. */

int main(int argc, char ** argv)
{
    int index; /* Index used to iterate over the array. */
    bool sorted; /* Bool used to make sure the array really is sorted. */
    double start_time, end_time; /* Timing values. */

    /* Reads the desired time if any was specified. If it was larger than allowed size
     * sets it to MAXSIZE. */
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

    vector = malloc(sizeof(int) * size); /* Allocates space for the array on the heap. */
    
    srand(time(NULL)); /* Seeds the randomizer to provide different results every time. */
    for(index = 0; index < size; index++) /* Initialize the array with random numbers. */
    {
        vector[index] = rand();
#ifdef DEBUG
        printf("%d: %d\n", index, vector[index]);
#endif
    }
#ifdef DEBUG
    printf("--------- Sorting! ---------\n");
#endif
    start_time = omp_get_wtime(); /* Read the time for the first time. */

    /* Important part here!
     *
     * First creates a parallel region. This will spawn a number of threads.
     * But because we only want to sort the array once we only want to run the sorting of
     * the entire array once. Hence we declare this to be single. The thread running the
     * single part will then spawn more tasks which will be executed in parallel. 
     */
#pragma omp parallel
    {
#pragma omp single nowait
        {
            quicksort(0, size);
        }
    }
    /* End of parallel region. */

    end_time = omp_get_wtime(); /* Reads the time after the parallel region is done. */
#ifdef DEBUG
    printf("------ Done sorting! -------\n");
#endif
    sorted = true; /* Default is that the array was sorted correctly. */
    for(index = 0; index < size; index++)
    {
        if(index != 0)
        {
            if(vector[index] < vector[index - 1]) /* If we find an element that is lesser than
                                                     the one before the array was not sorted. */
                sorted = false;
        }
#ifdef DEBUG
        printf("%d: %d\n", index, vector[index]);
#endif
    }

    free(vector); /* Free the vector. */

    /* Prints feedback to the user if the array was correctly sorted. */
    if(sorted)
        printf("The vector was sorted!\n");
    else
        printf("Sorting failed!\n");
    /* Prints the time it took to sort the array. */
    printf("Program took %g seconds to sort.\n", end_time - start_time);
    return(EXIT_SUCCESS);
}

void swap_place(int a, int b)
{
    int tmp;
    if(a == b) return;
    tmp = vector[a];
    vector[a] = vector[b];
    vector[b] = tmp;
}

void quicksort(int start, int end)
{
    if(end - start < BREAKPOINT)
    { /* Insertionsort. */
        int i;
        for(i = start + 1; i < end; i++)
        {
            int value_to_insert = vector[i];
            int hole_pos = i;
            while(hole_pos > start && value_to_insert < vector[hole_pos - 1])
            {
                vector[hole_pos] = vector[hole_pos - 1];
                hole_pos--;
            }
            vector[hole_pos] = value_to_insert;
        }
    }
    else
    { /* Quicksort. */
        int pivot = vector[end - 1]; /* NOT end because that points to one after last element. */
        int store_index = start;
        int i;
        for(i = start; i < end - 1; i++)
        {
            if(vector[i] < pivot)
            {
                swap_place(i, store_index);
                store_index++;
            }
        }

        swap_place(end - 1, store_index);

#pragma omp task
        {
            quicksort(start, store_index);
        }
#pragma omp task
        {
            quicksort(store_index + 1, end);
        }
    }
}
