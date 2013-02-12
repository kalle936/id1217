#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SHARED 1
#define MAXMALES 10
#define MAXWOMEN 10
#define MAXTIMES 100
#define MAXTIMEIN 3
#define MINTIMEIN 1

void * male(void *);
void * female(void *);

sem_t crit_sem, male_sem, female_sem;
int males_inside, females_inside, males_waiting, females_waiting;

int times_used;

int males, women;

int main(int argc, char ** argv)
{
    pthread_attr_t attr;
    pthread_t persons[MAXMALES + MAXWOMEN];
    int index;

    if(argc < 3)
    {
        printf("Usage ./unisex.out MALES WOMEN\n");
        return EXIT_SUCCESS;
    }
    else
    {
        males = atoi(argv[1]);
        if(males > MAXMALES) males = MAXMALES;
        women = atoi(argv[2]);
        if(women > MAXWOMEN) women = MAXWOMEN;
    }



    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    sem_init(&crit_sem, SHARED, 1);
    sem_init(&male_sem, SHARED, 0);
    sem_init(&female_sem, SHARED, 0);

    males_inside = 0;
    females_inside = 0;
    males_waiting = 0;
    females_waiting = 0;

    srand(time(NULL));

    for(index = 0; index < males; index++)
    {
        pthread_create(&persons[index], &attr, male, NULL);
    }
    for(index = 0; index < women; index++)
    {
        pthread_create(&persons[males + index], &attr, female, NULL);
    }
    for(index = 0; index < males + women; index++)
    {
        pthread_join(persons[index], NULL);
    }
    printf("Hej\n");
    return EXIT_SUCCESS;
}

void * male(void * input)
{
    while(times_used < MAXTIMES)
    {
        sem_wait(&crit_sem);
        if(females_inside > 0 || females_waiting > 0)
        {
            males_waiting++;
            sem_post(&crit_sem);
            sem_wait(&male_sem);
        }
        males_inside++;
        if(males_waiting > 0)
        {
            males_waiting--;
            sem_post(&male_sem);
        }
        else
        {
            sem_post(&crit_sem);
        }
        /* Simulate going to the toilet. */
        sleep((rand() % (MAXTIMEIN - MINTIMEIN)) + MINTIMEIN);

        sem_wait(&crit_sem);
        males_inside--;
        times_used++;
        if(males_inside == 0 && females_waiting > 0)
        {
            /* females_waiting--; */
            sem_post(&female_sem);
        }
        else
        {
            sem_post(&crit_sem);
        }
    }
    pthread_exit(NULL);
}

void * female(void * input)
{
    while(times_used < MAXTIMES)
    {
    }
    pthread_exit(NULL);
}
