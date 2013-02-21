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
#define MAX_WAIT_TIME 10     /* Max time between visits.     */
#define MIN_WAIT_TIME 2     /* Min time between visits.     */

void * man(void *);         /* Simulation method for men.   */
void * woman(void *);       /* Simulation method for women. */

/* Counters to hold number of men, women and the number of
 * times the bathroom has been used. */
int men, women, times_used;

pthread_mutex_t counter_lock;

monitor * mon;

int main(int argc, char ** argv)
{
    pthread_t persons[MAX_MEN + MAX_WOMEN];
    pthread_attr_t attr;

    men = MAX_MEN;
    women = MAX_WOMEN;
    times_used = 0;

    if(argc < 3)
    {
        std::cout << "Usage ./unisex NRW NRM" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        women = (atoi(argv[1]) > MAX_WOMEN) ? MAX_WOMEN : atoi(argv[1]);
        men = (atoi(argv[2]) > MAX_MEN) ? MAX_MEN : atoi(argv[2]);
    }

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    pthread_mutex_init(&counter_lock, NULL);

    srand(time(NULL));

    mon = new monitor();
    for(int i = 0; i < men; ++i)
    {
        pthread_create(&persons[i], &attr, man, NULL);
    }
    for(int i = 0; i < men; ++i)
    {
        pthread_create(&persons[men + i], &attr, woman, NULL);
    }
    for(int i = 0; i < men + women; ++i)
    {
        pthread_join(persons[i], NULL);
    }
    delete mon;
    return EXIT_SUCCESS;
}

void * woman(void * input)
{
    while(true)
    {
        /* Sleep to simulate time before a bathroom visit is necessary. */
        sleep((rand() % (MAX_WAIT_TIME - MIN_WAIT_TIME)) + MIN_WAIT_TIME);

        if(times_used >= MAX_TIMES)
        {
            pthread_exit(NULL);
        }

        /* Enter the bathroom. */
        mon->woman_enter();

        /* Update times_used. */
        pthread_mutex_lock(&counter_lock);
        times_used++;
        pthread_mutex_unlock(&counter_lock);

        /* Sleep to simulate a bathroom visit. */
        sleep((rand() % (MAX_TOILET_TIME - MIN_TOILET_TIME)) + MIN_TOILET_TIME);

        /* Exit the bathroom. */
        mon->woman_exit();
    }
}

void * man(void * input)
{
    while(true)
    {
        /* Sleep to simulate time before a bathroom visit is necessary. */
        sleep((rand() % (MAX_WAIT_TIME - MIN_WAIT_TIME)) + MIN_WAIT_TIME);

        if(times_used >= MAX_TIMES)
        {
            pthread_exit(NULL);
        }

        /* Enter the bathroom. */
        mon->man_enter();

        /* Update times_used. */
        pthread_mutex_lock(&counter_lock);
        times_used++;
        pthread_mutex_unlock(&counter_lock);

        /* Sleep to simulate a bathroom visit. */
        sleep((rand() % (MAX_TOILET_TIME - MIN_TOILET_TIME)) + MIN_TOILET_TIME);

        /* Exit the bathroom. */
        mon->man_exit();
    }
}
