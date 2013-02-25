#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <iostream>
#include <pthread.h>
#include "monitor.h"
#include <stdbool.h>

#define MAX_BEES 20         /* Max number of bees.          */
#define MAX_TIMES 10        /* Max number of times eating.  */
#define MAX_GATHER_TIME 500 /* Max time of gathering honey. */
#define MIN_GATHER_TIME 100 /* Min time of gathering honey. */

void * bee(void *);         /* Simulation method for bees.   */
void * bear(void *);        /* Simulation method for bears. */

/* Counters to hold number of bees and the number of
 * times the bear has eaten. */
int bees, times_eaten;

/* Boolean to signal to the bees that the simulation is done. */
bool done;

/* The monitor for the bathroom. */
monitor * mon;

/**
 * Main method. First initializes and then reads the input of how
 * many bees that should be in the simulation. Then
 * creates the threads and then joins them again. */
int main(int argc, char ** argv)
{
    pthread_t animals[MAX_BEES + 1]; /* The threads are saved here. */
    pthread_attr_t attr; /* Attributes for the pthreads. */

    long index; /* For 64-bit systems. */

    /* Set counter of bathroom visits to 0. */
    times_eaten = 0;

    /* Simulation is not done by default. */
    done = false;

    if(argc < 2) /* If unsufficient amount of arguments were supplied. */
    {
        std::cout << "Usage ./feedthebear BEES" << std::endl;
        return EXIT_FAILURE;
    }
    else /* Read the number of bees */
    {
        /* We cap them at MAX_BEES. */
        bees = (atoi(argv[1]) > MAX_BEES) ? MAX_BEES : atoi(argv[1]);
    }

    /* Initialize the attributes. */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Seed the randomizer with the time to provide different result every run. */
    srand(time(NULL));

    mon = new monitor(); /* Create the monitor. */
    for(index = 0; index < bees; ++index) /* Spawn the bee threads. */
    {
        pthread_create(&animals[index], &attr, bee, (void *) index);
    }
    pthread_create(&animals[bees], &attr, bear, NULL);
    for(int i = 0; i < bees + 1; ++i) /* Joins the threads again. */
    {
        pthread_join(animals[i], NULL);
    }
    delete mon; /* Delete the monitor when done. */
    return EXIT_SUCCESS;
}

/**
 * Simulation method for the bees.
 *
 * It will sleep for a random amount of time to simulate the gathering of nectar to
 * produce honey. Then it will try to place the honey in the pot.
 *
 * The bee will check if the bear still wants more honey by checking the boolean done.
 *
 * If the amount of bees is lesser than the amount of portions in the pot this solution
 * is fair to the bees. Otherwise there could be starvation in a VERY unlucky case where
 * one bee is waiting for the bear to eat the honey, once the bear has eaten the honey
 * bees of the amount the same as the amount of portions of honey the pot can hold gets hold
 * of the lock before the signaled bee. This would cause the pot to be full again and the bee
 * who waited will have to signal to bear again. This could potentially continue forever and
 * cause starvation (however the bear would never starve). It should be noted that the
 * probability of this happening several times, let alone one time, is practically 0.
 *
 * This is remedied by not allowing the bees to be more than 20 and the pot to take 50 portions.
 */
void * bee(void * input)
{
    long number = (long) input;
    while(done == false)
    {
        /* Sleep to simulate time required to gather honey. */
        usleep((rand() % (MAX_GATHER_TIME - MIN_GATHER_TIME)) + MIN_GATHER_TIME);

        if(done) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }
        mon->fill_pot(number); /* Fills the pot with one portion of honey. */
        if(done) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

/**
 * Simulation method for the bear. Tries to eat the honey in the pot once is is full for a
 * total of MAX_TIMES times. Once the bear has eaten MAX_TIMES times the bear will set done to
 * true and exit.
 */
void * bear(void * input)
{
    while(true)
    {
        mon->eat_honey(); /* Eat honey from the pot. */
        /* Update times_eaten. */
        times_eaten++; /* ONLY the bear can update this, therefore we do not need mutex. */
        std::cout << "The bear has now eaten " << times_eaten << " out of " << MAX_TIMES 
            << " times." << std::endl;
        if(times_eaten >= MAX_TIMES) /* If we are done eating. */
        {
            done = true; /* Signal to the bees that we are done. */
            pthread_exit(NULL);
        }
    }
}
