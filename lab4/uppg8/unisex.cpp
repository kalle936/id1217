#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include "monitor.h"

#define MAX_WOMEN 20        /* Max number of women.         */
#define MAX_MEN 20          /* Max number of men.           */
#define MAX_TIMES 40        /* Max times inside bathroom.   */
#define MAX_TOILET_TIME 3   /* Max time inside bathroom.    */
#define MIN_TOILET_TIME 1   /* Min time in bathroom.        */
#define MAX_WAIT_TIME 10    /* Max time between visits.     */
#define MIN_WAIT_TIME 2     /* Min time between visits.     */

void * man(void *);         /* Simulation method for men.   */
void * woman(void *);       /* Simulation method for women. */

/* Counters to hold number of men, women and the number of
 * times the bathroom has been used. */
int men, women, times_used;

/* Lock for the counter to exit the simulation. */
pthread_mutex_t counter_lock;

/* The monitor for the bathroom. */
monitor * mon;

/**
 * Main method. First initializes and then reads the input of how
 * many men and women that should be in the simulation. Then
 * creates the threads and then joins them again. */
int main(int argc, char ** argv)
{
    pthread_t persons[MAX_MEN + MAX_WOMEN]; /* The threads are saved here. */
    pthread_attr_t attr; /* Attributes for the pthreads. */

    long index; /* For 64-bit systems. */

    /* Set counter of bathroom visits to 0. */
    times_used = 0;

    if(argc < 3) /* If unsufficient amount of arguments were supplied. */
    {
        std::cout << "Usage ./unisex NRW NRM" << std::endl;
        return EXIT_FAILURE;
    }
    else /* Read the number of men and women. */
    {
        /* We cap them at MAX_WOMEN and MAX_MEN. */
        women = (atoi(argv[1]) > MAX_WOMEN) ? MAX_WOMEN : atoi(argv[1]);
        men = (atoi(argv[2]) > MAX_MEN) ? MAX_MEN : atoi(argv[2]);
    }

    /* Initialize the attributes. */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Initialize the lock. */
    pthread_mutex_init(&counter_lock, NULL);

    /* Seed the randomizer with the time to provide different result every run. */
    srand(time(NULL));

    mon = new monitor(); /* Create the monitor. */
    for(index = 0; index < men; ++index) /* Spawn the men threads. */
    {
        pthread_create(&persons[index], &attr, man, (void *) index);
    }
    for(index = 0; index < men; ++index) /* Spawn the women threads. */
    {
        pthread_create(&persons[men + index], &attr, woman, (void *) index);
    }
    for(int i = 0; i < men + women; ++i) /* Joins the threads again. */
    {
        pthread_join(persons[i], NULL);
    }
    delete mon; /* Delete the monitor when done. */
    return EXIT_SUCCESS;
}

/**
 * Women method for simulating bathroom visits to a unisex toilet.
 *
 * First sleeps to simulate the time between bathroom breaks and then calls the
 * monitor method woman_enter. Then simulate the bathroom visit by sleeping and
 * finally exits the bathroom by calling the monitor method woman_exit.
 */
void * woman(void * input)
{
    long number = (long) input;
    while(true)
    {
        /* Sleep to simulate time before a bathroom visit is necessary. */
        sleep((rand() % (MAX_WAIT_TIME - MIN_WAIT_TIME)) + MIN_WAIT_TIME);

        if(times_used >= MAX_TIMES) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }

        /* Enter the bathroom. */
        mon->woman_enter(number);

        /* Sleep to simulate a bathroom visit. */
        sleep((rand() % (MAX_TOILET_TIME - MIN_TOILET_TIME)) + MIN_TOILET_TIME);

        /* Update times_used. */
        pthread_mutex_lock(&counter_lock);
        times_used++;
        pthread_mutex_unlock(&counter_lock);

        /* Exit the bathroom. */
        mon->woman_exit(number);
    }
}

/**
 * Men method for simulating bathroom visits to a unisex toilet.
 *
 * First sleeps to simulate the time between bathroom breaks and then calls the
 * monitor method man_enter. Then simulate the bathroom visit by sleeping and
 * finally exits the bathroom by calling the monitor method man_exit.
 */
void * man(void * input)
{
    long number = (long) input;
    while(true)
    {
        /* Sleep to simulate time before a bathroom visit is necessary. */
        sleep((rand() % (MAX_WAIT_TIME - MIN_WAIT_TIME)) + MIN_WAIT_TIME);

        if(times_used >= MAX_TIMES) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }

        /* Enter the bathroom. */
        mon->man_enter(number);

        /* Sleep to simulate a bathroom visit. */
        sleep((rand() % (MAX_TOILET_TIME - MIN_TOILET_TIME)) + MIN_TOILET_TIME);

        /* Update times_used. */
        pthread_mutex_lock(&counter_lock);
        times_used++;
        pthread_mutex_unlock(&counter_lock);

        /* Exit the bathroom. */
        mon->man_exit(number);
    }
}
