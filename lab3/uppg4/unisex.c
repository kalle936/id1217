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
#define MAXTIMES 10
#define MAXTIMEIN 3
#define MINTIMEIN 1
#define MAXINTERVAL 5
#define MININTERVAL 1

void * male(void *);
void * female(void *);

sem_t crit_sem, accessing_sem, waiting_sem;
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
    sem_init(&accessing_sem, SHARED, 1);
    sem_init(&waiting_sem, SHARED, 1);

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
    return EXIT_SUCCESS;
}

void * male(void * input)
{
    while(times_used < MAXTIMES)
    {
        /* We simulate time until we need to go to the bathroom. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL)) + MININTERVAL);

        printf("Male trying to enter bathroom.\n");
        sem_wait(&waiting_sem);
        if(females_inside > 0)
        {
            printf("Male waiting for females to exit bathroom.\n");
            sem_wait(&accessing_sem);
            printf("Now it is the males turn to enter the bathroom.\n");
        }
        else if(males_inside == 0)
        {
            printf("Nobody inside, hence the male is entering the bathroom.\n");
            sem_wait(&accessing_sem);
        }
        sem_wait(&crit_sem);
        males_inside++; /* We are now inside the bathroom. */
        printf("Male inside the bathroom!\n");
        sem_post(&crit_sem);
        sem_post(&waiting_sem);

        /* Simulate going to the toilet. */
        sleep((rand() % (MAXTIMEIN - MINTIMEIN)) + MINTIMEIN);

        sem_wait(&crit_sem);
        times_used++;
        males_inside--;
        printf("Male exiting bathroom.\n");
        if(males_inside == 0)
        {
            printf("Last male has exited bathroom.\n");
            sem_post(&accessing_sem);
        }
        sem_post(&crit_sem);


    }
    pthread_exit(NULL);
}

void * female(void * input)
{
    while(times_used < MAXTIMES)
    {
        /* We simulate time until we need to go to the bathroom. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL)) + MININTERVAL);

        printf("Female trying to enter bathroom.\n");
        sem_wait(&waiting_sem);
        if(males_inside > 0)
        {
            printf("Female waiting for males to exit bathroom.\n");
            sem_wait(&accessing_sem);
            printf("Now it is the females turn to enter the bathroom.\n");
        }
        else if(females_inside == 0)
        {
            printf("Nobody inside - female enter.\n");
            sem_wait(&accessing_sem);
        }
        sem_wait(&crit_sem);
        females_inside++; /* We are now inside the bathroom. */
        printf("Female inside the bathroom!\n");
        sem_post(&crit_sem);
        sem_post(&waiting_sem);

        /* Simulate going to the toilet. */
        sleep((rand() % (MAXTIMEIN - MINTIMEIN)) + MINTIMEIN);

        sem_wait(&crit_sem);
        times_used++;
        females_inside--;
        printf("Female exiting bathroom.\n");
        if(females_inside == 0)
        {
            printf("Last female has exited bathroom.\n");
            sem_post(&accessing_sem);
        }
        sem_post(&crit_sem);
    }
    pthread_exit(NULL);
}
