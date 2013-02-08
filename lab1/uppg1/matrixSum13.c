/* matrix summation using pthreads

   features: Uses shared variables to with locks to calculate sum, min and max 
            of the matrix. Uses a bag of tasks implemented as a counter to assign
            the workers rows to calculate the partial result and then updates the
            shared variable.

   usage under Linux:
            compile using make matrixSum13.c
            execute using ./matrixSum13.out size numWorkers

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

pthread_mutex_t counter_lock;   /* mutex lock for the counter */
pthread_mutex_t result_lock;    /* mutex lock for the result */
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
int size; /* assume size is multiple of numWorkers */
int sum; /* partial sums */
int max; /* max value */
int max_y; /* y pos for max */
int max_x; /* x pos for max */
int min; /* min value */
int min_y; /* y pos for min */
int min_x; /* x pos for min */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */
int counter; /* Shared variable that is used as a bag of task.*/

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
    int i, j;
    long l; /* use long in case of a 64-bit system */
    int correct_sum = 0; /* Used to make sure the result is correct. */
    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* initialize mutex and condition variable */
    pthread_mutex_init(&counter_lock, NULL);
    pthread_mutex_init(&result_lock, NULL);

    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    /* Feed the random seed the time to create different results. */
    srand(time(NULL));
    /* initialize the matrix */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            /* matrix[i][j] = 1; */
            matrix[i][j] = rand() % MAXNUMBER;
            correct_sum += matrix[i][j]; /* Calculate the correct sum */
        }
    }

  /* print the matrix */
#ifdef DEBUG
    if(size > 50)
    {
        printf("To large matrix to print.\n");
    }
    else {
        for (i = 0; i < size; i++) {
            printf("[ ");
            for (j = 0; j < size; j++) {
                printf(" %d", matrix[i][j]);
            }
            printf(" ]\n");
        }
    }
#endif

    /* Initializes the variables. */
    sum = 0;
    max = MINNUMBER - 1;
    min = MAXNUMBER + 1;
    max_x = -1;
    max_y = -1;
    min_x = -1;
    min_y = -1;
    counter = 0; /* Start at row number 0. */

    start_time = read_timer(); /* Read the timer for the first time. */
    for (l = 0; l < numWorkers; l++) /* Create the workers. */
    {
        pthread_create(&workerid[l], &attr, Worker, NULL);
    }

    for(l = 0; l < numWorkers; l++) /* Waits for the workers to terminate. */
    {
        pthread_join(workerid[l], NULL);
    }

    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", sum);
    printf("The total is supposed to be %d\n", correct_sum);
    printf("The min value is %d, and is located at (%d,%d).\n", min, min_y, min_x);
    printf("The max value is %d, and is located at (%d,%d).\n", max, max_y, max_x);
    printf("The execution time is %g sec\n", end_time - start_time);

    pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix. */
void *Worker(void *arg) {
    int total, j, local_min, local_min_x, local_min_y, local_max, local_max_x, local_max_y, row;
    total = 0;
    local_min_x = 0;
    local_min_y = 0;
    local_max_x = 0;
    local_max_y = 0;
    local_min = MAXNUMBER + 1; /* It is ok to omitt the +1 but for readability I left it in 
                                * (This is because % MAXVALUE does not include MAXVALUE). */
    local_max = MINNUMBER - 1; /* Here the - 1 is needed if the matrix is just minvalue. */
    while(true)
    {
        /* Lock the counter lock and read the counter and update the counter, then unlock again.
         * It is important that this is atomic because two workers should not work on the same
         * row. */
        pthread_mutex_lock(&counter_lock);
        row = counter;
        counter++;
        pthread_mutex_unlock(&counter_lock);

        /* If we are done we update the shared variables. A lock is required here aswell. */
        if(row >= size)
        {
            pthread_mutex_lock(&result_lock);
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
            pthread_mutex_unlock(&result_lock);
            pthread_exit(NULL);
        }
        else
        {
            /* Calculate sum, min and max for the row. */
            for (j = 0; j < size; j++)
            {
                int matrix_value = matrix[row][j];
                total += matrix_value; /* Update local sum. */
                if(matrix_value < local_min) /* Update local min if needed. */
                {
                    local_min = matrix_value;
                    local_min_y = row;
                    local_min_x = j;
                }
                if(matrix_value > local_max) /* Update local max if needed. */
                {
                    local_max = matrix_value;
                    local_max_y = row;
                    local_max_x = j;
                }
            }
        }
    }
}
