/* matrix summation, finding min and max using pthreads

   features: Does not use a barrier in this version but joins the threads
            and then prints the result in the main thread by using shared
            variables.

   usage under Linux:
            compile using make matrixSum12.c
            execute using ./matrixSum12.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define MAXNUMBER 99 /* Largest possible number in the matrix + 1 */
#define MINNUMBER 0 /* Smallest possible number in the matrix */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sum; /* partial sums */
int max; /* max value */
int max_y; /* y pos for max */
int max_x; /* x pos for max */
int min; /* min value */
int min_y; /* y pos for min */
int min_x; /* x pos for min */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
    int i, j;
    long l; /* use long in case of a 64-bit system */
    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* initialize mutex and condition variable */
    pthread_mutex_init(&barrier, NULL);
    pthread_cond_init(&go, NULL);

    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    stripSize = size/numWorkers;

    srand(time(NULL));
    /* initialize the matrix */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            /* matrix[i][j] = 1; */
            matrix[i][j] = rand() % MAXNUMBER;
        }
    }

  /* print the matrix */
#ifdef DEBUG
    for (i = 0; i < size; i++) {
        printf("[ ");
        for (j = 0; j < size; j++) {
            printf(" %d", matrix[i][j]);
        }
        printf(" ]\n");
    }
#endif

    /* Inits the variables. */
    sum = 0;
    max = MINNUMBER - 1;
    min = MAXNUMBER + 1;
    max_x = -1;
    max_y = -1;
    min_x = -1;
    min_y = -1;

    start_time = read_timer(); /* Read the timer the first time. */
    for (l = 0; l < numWorkers; l++) /* Starts the workers. */
    {
        pthread_create(&workerid[l], &attr, Worker, (void *) l);
    }

    for(l = 0; l < numWorkers; l++) /* Waits for the workers to complete. */
    {
        pthread_join(workerid[l], NULL);
    }

    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", sum);
    printf("The min value is %d, and is located at (%d,%d).\n", min, min_y, min_x);
    printf("The max value is %d, and is located at (%d,%d).\n", max, max_y, max_x);
    printf("The execution time is %g sec\n", end_time - start_time);

    pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix. */
void *Worker(void *arg) {
    long myid = (long) arg; /* Using long because of 64-bit systems. */
    /* Private variables. */
    int total, i, j, first, last, local_min, local_min_x, local_min_y, local_max, local_max_x, local_max_y;

#ifdef DEBUG
    printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

    /* determine first and last rows of my strip */
    first = myid*stripSize;
    last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

    /* Initialize the private variables. */
    total = 0; /* sum values in my strip */
    local_min_x = -1;
    local_min_y = -1;
    local_max_x = -1;
    local_max_y = -1;
    local_min = MAXNUMBER + 1; /* It is ok to omitt the +1 but for readability I left it in (This is because % MAXVALUE does not include MAXVALUE). */
    local_max = MINNUMBER - 1; /* Here the - 1 is needed if the matrix is just minvalue. */
    for (i = first; i <= last; i++) /* Iterate over the assigned part of the matrix */
    {
        for (j = 0; j < size; j++)
        {
            int matrix_value = matrix[i][j];
            total += matrix_value; /* Update the local sum. */
            if(matrix_value < local_min) /* If the value is lower than the local min. */
            {
                local_min = matrix_value;
                local_min_y = i;
                local_min_x = j;
            }
            if(matrix_value > local_max) /* If the value is larger than the local max. */
            {
                local_max = matrix_value;
                local_max_y = i;
                local_max_x = j;
            }
        }
    }

    /* When updating the shared variable a lock is needed. */
    pthread_mutex_lock(&barrier);
    sum += total; /* Update sum. */
    if(min > local_min) /* Update min if needed. */
    {
        min = local_min;
        min_x = local_min_x;
        min_y = local_min_y;
    }
    if(max < local_max) /* Update max if needed. */
    {
        max = local_max;
        max_x = local_max_x;
        max_y = local_max_y;
    }
    pthread_mutex_unlock(&barrier); /* We are done with the shared variables. */
    pthread_exit(NULL);
}
