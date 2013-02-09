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
#include <time.h>
#include <stdbool.h>

#define MAXSIZE 1000000 /* Maximum size of array to be sorted. */
#define BREAKPOINT 50 /* Breakpoint when insertion sort is made instead of quicksort. */
void quicksort(int, int);

int size;
int * vector;

int main(int argc, char ** argv)
{
    int index;
    bool sorted;
    double start_time, end_time;
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

    srand(time(NULL));
    for(index = 0; index < MAXSIZE; index++)
    {
        vector[index] = rand();
#ifdef DEBUG
        printf("%d: %d\n", index, vector[index]);
#endif
    }
#ifdef DEBUG
    printf("--------- Sorting! ---------\n");
#endif
    start_time = omp_get_wtime();
#pragma omp parallel
    {
#pragma omp single nowait
        {
            quicksort(0, size);
        }
    }
    end_time = omp_get_wtime();
#ifdef DEBUG
    printf("------ Done sorting! -------\n");
#endif
    sorted = true;
    for(index = 0; index < MAXSIZE; index++)
    {
        if(index != 0)
        {
            if(vector[index] < vector[index - 1])
                sorted = false;
        }
#ifdef DEBUG
        printf("%d: %d\n", index, vector[index]);
#endif
    }

    free(vector);

    if(sorted)
        printf("The vector was sorted!\n");
    else
        printf("Sorting failed!\n");
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
