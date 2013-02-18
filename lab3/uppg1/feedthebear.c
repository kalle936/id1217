/**
 * Simulates several bees and a bear. The bees gather honey and the bear
 * eats the honey when the pot of honey is full. 
 *
 * Compile with make file using "make feedthebear.out" (run rule will compile
 * if needed as well.
 *
 * Run through make file using default rule (run). Specify number of bees through
 * setting BEES to the amount of bees wanted.
 *
 * Example:
 * > BEES=30 make
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define SHARED 0        /* The semaphores should not be shared. */
#define MAXBEES 50      /* Max number of bees in this simulation. */
#define MAXINTERVAL 3   /* Max time it takes to gather honey. */
#define MININTERVAL 1   /* Min time it takes to gather honey. */
#define MAXEATING 10    /* Max number of times the bear eats. */
#define CAPACITY 50     /* Number of portions of honey the pot can take. */

void * bear(void *); /* Declaration of the bear method. */
void * bee(void *); /* Declaration of the bee method. */

sem_t crit_sem, pot_empty, pot_full; /* Semaphores used. */

/* Counters used. */
int honey_counter, times_eaten;

int bees; /* Number of bees in the simulation. */

/**
 * Main method. Creates the bees and the bear and then joins them again when
 * the workload is done.
 */
int main(int argc, char ** argv)
{
    pthread_attr_t attr; /* Pthread attributes. */
    pthread_t animals[MAXBEES + 1]; /* Threads that we should start. */
    int index; /* Used to iterate over the threads. */

    if(argc < 2) /* Check if insufficient number of arguments has been entered. */
    {
        printf("Usage ./feedthebear.out BEES\n");
        return EXIT_SUCCESS;
    }
    else /* Sets amount of bees to be min(MAXBEES, argv[1]). */
    {
        bees = atoi(argv[1]);
        if(bees > MAXBEES) bees = MAXBEES;
    }

    /* Initialize the pthread attribute. */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Initialize the semaphores used. */
    sem_init(&crit_sem, SHARED, 1);
    sem_init(&pot_empty, SHARED, 0);
    sem_init(&pot_full, SHARED, 0);

    /* Initialize the counters. */
    honey_counter = 0;
    times_eaten = 0;

    srand(time(NULL)); /* Seed the randomizer to provide different results. */

    for(index = 0; index < bees; index++) /* Creates the bees. */
    {
        pthread_create(&animals[index], &attr, bee, NULL);
    }
    pthread_create(&animals[bees], &attr, bear, NULL); /* Creates the bear. */
    for(index = 0; index < bees + 1; index++) /* Joins the threads again. */
    {
        pthread_join(animals[index], NULL);
    }
    return EXIT_SUCCESS; /* Once we joined the thread we exit. */
}

/**
 * Bee simulation method. Gathers honey for the bear until the bear does not want
 * anymore honey. This is signaled as when times_eaten >= MAXEATING.
 *
 * Simulates gathering honey with the thread sleeping. If the bee is responsible
 * for making the pot of honey full it will wake the bear and pass the baton to it.
 * Then the bee will wait for the bear to eat the honey and then pass the baton back
 * so the bee can continue gathering honey. This will ensure that the solution is fair
 * towards the bees (and the bear) (more explanation in README.md).
 */
void * bee(void * input)
{
    while(true)
    {
        /* Simulate gathering honey. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL)) + MININTERVAL);

        sem_wait(&crit_sem); /* Begin critical section. */
        if(times_eaten >= MAXEATING) /* Bear is done eating. */ 
        {
            /* End critical region and exit. */
            sem_post(&crit_sem);
            break;
        }
        honey_counter++; /* Place honey in pot. */
        /* Prints information. */
        printf("Bee %d has gathered honey (%d/%d).\n", pthread_self(), honey_counter, CAPACITY);
        if(honey_counter >= CAPACITY) /* If the pot of honey is full. */
        {
            /* Prints information about waking the bear. */
            printf("Honey pot is full, waking bear!\n");
            sem_post(&pot_full); /* Pass the baton to the bear. */
            sem_wait(&pot_empty); /* Wait for the bear to pass the baton back. */
        }
        sem_post(&crit_sem); /* End critical section. */

    }
    pthread_exit(NULL); /* Exit thread. */
}

/**
 * Bear simulation method. Sleeps (waits) for the pot to be full and then eats it.
 * Repeat this procedure until bear has eaten MAXEATEN times.
 *
 * The bee waking the bear still has crit_sem and is now waiting for the pot to
 * be empty. Therefore we do not need to lock for critical section. This technique
 * is called pass the baton. We pass the baton back to the bee waking us and this
 * bee will be responsible to exit critical section.
 */
void * bear(void * input)
{
    while(times_eaten < MAXEATING) /* Eating a maximum of CAPACITY times. */
    {
        sem_wait(&pot_full); /* Wait for the pot to be full. */
        times_eaten++; /* Update counter of times eaten. */
        /* Alert that the bear is now awake. */
        printf("Bear is now awake and starts eating (%d/%d).\n", times_eaten, MAXEATING);
        honey_counter = 0; /* Pot is now empty. */
        /* Alert that the bear has now finished eating. */
        printf("Bear finished the honey. Going back to sleep.\n");
        sem_post(&pot_empty); /* Pass the baton back to the bee. */
    }
    /* Let user know that the bear does not want anymore honey. */
    printf("Bear is now done eating and wants to play with his moose friends.\n");
    pthread_exit(NULL); /* Exit thread. */
}
