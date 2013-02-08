/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
            compile using make matrixSum11.c
            execute using ./matrixSum11.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
        return 0;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int max[MAXWORKERS]; /* max value */
int max_y[MAXWORKERS]; /* y pos for max */
int max_x[MAXWORKERS]; /* x pos for max */
int min[MAXWORKERS]; /* min value */
int min_y[MAXWORKERS]; /* y pos for min */
int min_x[MAXWORKERS]; /* x pos for min */
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

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
          /* matrix[i][j] = 1; */
          matrix[i][j] = rand() % 99;
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

  /* do the parallel work: create the workers */
    start_time = read_timer();
    for (l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], &attr, Worker, (void *) l);
    pthread_exit(NULL);

}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
    long myid = (long) arg; /* The id of the worker, decide which part it will calculate. */

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
    /* This is ONLY ok when values are between 0 and 99 */
    local_min = 100;
    local_max = -1;
    /* Iterate over the assigned rows in the matrix. */
    for (i = first; i <= last; i++)
    {
        for (j = 0; j < size; j++)
        {
            int matrix_value = matrix[i][j];
            total += matrix_value; /* Local total value is updated. */
            if(matrix_value < local_min) /* If this is the smallest value so far. */
            {
                local_min = matrix_value;
                local_min_y = i;
                local_min_x = j;
            }
            if(matrix_value > local_max) /* If this is the largest value so far. */
            {
                local_max = matrix_value;
                local_max_y = i;
                local_max_x = j;
            }
        }
    }
    /* Writes the local values to the arrays. */
    sums[myid] = total;
    min[myid] = local_min;
    min_x[myid] = local_min_x;
    min_y[myid] = local_min_y;
    max[myid] = local_max;
    max_x[myid] = local_max_x;
    max_y[myid] = local_max_y;
    
    Barrier(); /* Wait for the rest of the workers. */
    if (myid == 0) { /* Only worker 0 shall print the result. */
        /* Initialize the total values with the result from worker 0. */
        int tot_max, tot_max_x, tot_max_y, tot_min, tot_min_x, tot_min_y;
        total = sums[0];
        tot_max = max[0];
        tot_max_x = max_x[0];
        tot_max_y = max_y[0];
        tot_min = min[0];
        tot_min_x = min_x[0];
        tot_min_y = min_y[0];
        /* Iterate over the rest of the workers. */
        for (i = 1; i < numWorkers; i++)
        {
            total += sums[i];
            if(tot_min > min[i]) /* If the local min was lower than the total min. */
            {
                tot_min = min[i];
                tot_min_x = min_x[i];
                tot_min_y = min_y[i];
            }
            if(tot_max < max[i]) /* If the local max was higher than the total max. */
            {
                tot_max = max[i];
                tot_max_x = max_x[i];
                tot_max_y = max_y[i];
            }
            
        }
        /* get end time */
        end_time = read_timer();
        /* print results */
        printf("The total is %d\n", total);
        printf("The min value is %d, and is located at (%d,%d).\n", tot_min, tot_min_y, tot_min_x);
        printf("The max value is %d, and is located at (%d,%d).\n", tot_max, tot_max_y, tot_max_x);
        printf("The execution time is %g sec\n", end_time - start_time);
    }
    pthread_exit(NULL);
}
