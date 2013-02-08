/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */
#define MAXNR 100
#define MINNR 0

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
    int i, j, total=0, min=MAXNR, max=MINNR -1, min_y=-1, min_x=-1, max_y=-1, max_x=-1;

    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    omp_set_num_threads(numWorkers);

    srand(time(NULL));
    /* initialize the matrix */
    for (i = 0; i < size; i++) {
        printf("[ ");
        for (j = 0; j < size; j++) {
            matrix[i][j] = (rand() % MAXNR) + MINNR;
            printf(" %d", matrix[i][j]);
        }
        printf(" ]\n");
    }

    start_time = omp_get_wtime();
    #pragma omp parallel for reduction (+:total) private(j)
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            total += matrix[i][j];
            if(matrix[i][j] < min)
            {
                #pragma omp critical
                {
                    if(matrix[i][j] < min)
                    {
                        min = matrix[i][j];
                        min_x = j;
                        min_y = i;
                    }    
                }
            }
            if(matrix[i][j] > max)
            {
                #pragma omp critical
                {
                    if(matrix[i][j] > max)
                    {
                        max = matrix[i][j];
                        max_x = j;
                        max_y = i;
                    }
                }
            }
        }
    }
    /* implicit barrier */

    end_time = omp_get_wtime();

    printf("The total is %d.\n", total);
    printf("Maximum is %d, and is located at %d,%d.\n", max, max_y, max_x);
    printf("Minimum is %d, and is located at %d,%d.\n", min, min_y, min_x);
    printf("It took %g seconds\n", end_time - start_time);
    return(EXIT_SUCCESS); /* If omitted error codes 23, 26 or 27 are returned to make. */
}
