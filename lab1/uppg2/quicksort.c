/**
 * Quicksort that uses pthread for parallel execution.
 * 
 * Mocks its own input by using the function rand() in stdlib.
 * Example usage is ./quicksort.out 10000 5
 * This will sort an array with 10 000 elements by using 5 threads.
 *
 * Author: Karl Johan Andreasson
 */


#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>

// Maximum size of the array that is supported.
#define MAXSIZE 100000000

// On parts of the array that is smaller than BREAKPOINT the program
// will perform insertion sort instead of quicksort. This gives some speedup.
#define BREAKPOINT 50

// Maximum workers that is supported.
#define MAXWORKERS 10


/**
 * Struct that acts as a linked list with information on which part
 * of the array that should be sorted by the thread.
 *
 * Start is inclusive but end is not.
 * ie sort all x, start <= x < end.
 * (Mimics behaviour of iterators in C++ stl)
 */
struct task
{
    unsigned int start;
    unsigned int end;
    struct task * next;
};

typedef struct task * task_p; // Typedefining pointer to the struct for better code.
typedef struct task task_t; // Typedefining the struct for more understandable code.

// Lock to make sure only one thread works on the same part.
pthread_mutex_t bag_lock;

// Used to signal to the other threads that there new parts to be sorted.
pthread_cond_t bag_go;

int vector[MAXSIZE]; // The vector to be sorted.
task_p first; // Pointer to the first task.
task_p last; // Pointer to the last task (for fast insertion).
int size; // Size of the vector.
int workers; // Number of workers (threads sorting).
int workers_arrived; // Numbers of idle workers.

/**
 * Read the clock.
 *
 * First time returns 0. Subsequent calls returns time from first call.
 */
double read_timer()
{
    static bool initialized = false; // To determine if the call is the first or not.
    static struct timeval start; // Timeval to hold the time for the first call
    if(!initialized) // Read the clock for the first time.
    {
        gettimeofday(&start, NULL);
        initialized = true;
        return 0;
    }
    else // Read the clock, calculate the time from first call and returns it.
    {
        struct timeval end;
        gettimeofday(&end, NULL);
        return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
    }
}

// Declare the method worker for later usage.
void * worker(void *);


/**
 * Main method - initializes everything and then starts the threads to do the
 * sorting. Then waits for the workers to terminate and then makes sure the
 * vector is sorted and prints the time it took to sort.
 *
 * Example usage:
 * ./quicksort
 * Sorts the vector of default size with default number of threads.
 * ./quicksort 10000
 * Sorts the vector of size 10000 with the default number of threads.
 * ./quicksort 100000 8
 * Sorts the vector of size 100000 using 8 threads.
 */
int main(int argsc, char ** argsv)
{
    int i; // Index used to iterate over the vector.
    size = MAXSIZE; // The size is initially set to default.
    workers = MAXWORKERS; // Number of workers is initially set to default.

    workers_arrived = 0; // Initialize the counter for idle workers.

    if(argsc > 2) // Sets the number of workers.
    {
        workers = atoi(argsv[2]);
        if(workers > MAXWORKERS)
        {
            workers = MAXWORKERS;
        }
    }
    if(argsc > 1) // Sets the size of the vector.
    {
        size = atoi(argsv[1]);
        if(size > MAXSIZE)
        {
            size = MAXSIZE;
        }
    }

    // Initializing for using pthread.
    pthread_attr_t attr;
    pthread_t workerid[workers];

    // Set global thread attribute.
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    // Initialize the lock and the cond
    pthread_mutex_init(&bag_lock, NULL);
    pthread_cond_init(&bag_go, NULL);
    
    // Initialize the vector that is supposed to be sorted.
    srand(time(NULL)); // To make sure different numbers are used between different runs.
    for(i = 0; i < size; i++)
    {
        vector[i] = rand();
    }

    // Create the first task (this is the complete list).
    first = malloc(sizeof(task_t));
    last = first;
    first->start = 0;
    first->end = size;
    first->next = NULL;
 
    read_timer(); // Reads the clock the first time.

    // Creates the threads.
    for(i = 0; i < workers; i++)
    {
        pthread_create(&workerid[i], &attr, worker, NULL); 
    }

    // Joins them again. When this is done the sorting is complete.
    for(i = 0; i < workers; i++)
    {
        pthread_join(workerid[i], NULL);
    }

    double time = read_timer(); // Reads the clock the second time. The time elapsed is saved in time.

    // See if the vector really is sorted.
    bool accurate = true;
    for(i = 1; i < size; i++)
    {
        if(vector[i - 1] > vector[i])
        {
            accurate = false;
        }
    }
    if(accurate)
    {
        printf("The array was sorted correctly!\n");
    }
    else
    {
        printf("The array was NOT sorted correctly!\n");
    }
    printf("The program needed %g seconds to sort the array.\n", time);

    pthread_exit(NULL);
}

/**
 * Swaps the place of the integers on place i and j.
 */
void swap_place(int i, int j)
{
    // If they are the same we return immediately. 
    // This happens in quicksort in the initial stage.
    if(i == j) return;

    int tmp = vector[i];
    vector[i] = vector[j];
    vector[j] = tmp;
}

/**
 * Sorts the part of the vector assigned in by the task structs.
 * 
 * When all workers are idle the whole vector is sorted and the
 * program returns.
 */
void * worker(void * arg)
{
    task_p tp = NULL; // The current task
    bool done = false; // If the vector is sorted.

    while(true)
    {
        pthread_mutex_lock(&bag_lock); // Locks to make sure mutual exclusion.
        while(first == NULL && !done) // Tries to get a task.
        {
            workers_arrived++; // This worker is idle.
            if(workers_arrived >= workers) // If everyone are idle, the vector is sorted.
            {
                pthread_cond_broadcast(&bag_go); // Wakes the other threads so they can terminate.
                done = true; // We are done.
            }
            else
            {
                pthread_cond_wait(&bag_go, &bag_lock); // Waits for another task or that the vector is sorted.
            }
        }
        if(first != NULL) // Grab the first element in the linked array.
        {
            tp = first;
            first = first->next;
        }
        pthread_mutex_unlock(&bag_lock); // Unlocks the lock
        if(done) // Terminate if the vecor is sorted.
        {
            pthread_exit(NULL);
        }

        // Now sorting on the span assigned in the task.
        if(tp-> end - tp->start < BREAKPOINT) // Determine if quicksort or insertionsort should be used.
        {
            // Insertionsort
            int i; // Index used to traverse the vector.
            for(i = tp->start + 1; i < tp->end; i++) // first element is by definition sorted.
            {
                int value_to_insert = vector[i];
                int hole_pos = i;

                while( hole_pos > tp->start && value_to_insert < vector[hole_pos - 1]) // Find the elements place
                {
                    vector[hole_pos] = vector[hole_pos - 1];
                    hole_pos--;
                }
                vector[hole_pos] = value_to_insert;
            }
        }
        else
        {
            // Quicksort

            /* NOTE: This is ok for randomized input.
             * For sorted input this gives really bad performance. */
            int pivot = vector[tp->end - 1]; // Use last element as pivot.

            // To keep place where the split between elements smaller and larger or equal ones.
            int store_index = tp->start;
            int i; // Index to traverse the vector.
            for(i = tp->start; i < tp->end - 1; i++) // Last element is the pivot value.
            {
                if(vector[i] < pivot) // If smaller we place it at store_index and increase store_index.
                {
                    swap_place(i, store_index);
                    store_index++;
                }
            }

            // Place the pivot value at its place.
            swap_place(tp->end - 1, store_index);

            // Create 2 new tasks. One for the element below pivot
            // and one for greater or equal to the pivot.
            task_p task1 = malloc(sizeof(task_t));
            task_p task2 = malloc(sizeof(task_t));

            task1->start = tp->start;
            task1->end = store_index;
            task1->next = task2;
            
            task2->start = store_index + 1;
            task2->end = tp->end;
            task2->next = NULL;

            // To place the tasks in the bag of tasks we need mutual exclusion. Hence we lock.
            pthread_mutex_lock(&bag_lock);
            if(first == NULL) // If this is the only tasks present.
            {
                first = task1;
                last = task2;
            }
            else // Else place them in the back of the list.
            {
                last->next = task1;
                last = task2;
            }
            pthread_cond_broadcast(&bag_go); // Broadcasts that there are new tasks
            workers_arrived = 0; // No idle workers.
            pthread_mutex_unlock(&bag_lock); // Unlock

        }

        free(tp); // We free the memory of the last task used.
    }
}
