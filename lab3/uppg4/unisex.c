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

#define SHARED 0
#define MAXMALES 10
#define MAXWOMEN 10
#define MAXTIMES 20
#define MAXTIMEIN 3
#define MINTIMEIN 1
#define MAXINTERVAL 5
#define MININTERVAL 1

void * male(void *);
void * female(void *);

sem_t crit_sem, waiting_male_sem, waiting_female_sem;
int males_inside, females_inside, males_waiting, females_waiting;

int times_used;

bool letting_in_people;

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
    sem_init(&waiting_male_sem, SHARED, 0);
    sem_init(&waiting_female_sem, SHARED, 0);

    males_inside = 0;
    females_inside = 0;
    males_waiting = 0;
    females_waiting = 0;

    letting_in_people = false;

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

void print_status()
{
    printf("\nINSIDE: M=%d, F=%d.\nWAITING: M=%d, F=%d.\n\n", males_inside, females_inside, males_waiting, females_waiting);
}

void * male(void * input)
{
    int i;
    int males_in_queue;
    while(times_used < MAXTIMES)
    {
        /* We simulate time until we need to go to the bathroom. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL)) + MININTERVAL);

        sem_wait(&crit_sem);
        males_waiting++;
        printf("Male trying to enter bathroom.\n");
        print_status();

        if(females_inside > 0 || females_waiting > 0)
        {
            printf("Males waiting for their turn.\n");
            print_status();
            sem_post(&crit_sem);
            sem_wait(&waiting_male_sem); /* Waiting for our turn. */
            males_inside++; /* We use pass the baton technique. */
            males_waiting--;
            if(letting_in_people == false)
            {
                letting_in_people = true;
                males_in_queue = males_waiting;
                printf("Males turn, first one entering.\n");
                print_status();
                for(i = 0; i < males_in_queue; i++)
                {
                    sem_post(&waiting_male_sem);
                    sem_wait(&crit_sem);
                }
                letting_in_people = false;
            }
            else
            {
                printf("Male entering.\n");
                print_status();
            }

        }
        else /* Females are already inside. */
        {
            males_inside++;
            males_waiting--;
            printf("Male entering.\n");
            print_status();
        }
        sem_post(&crit_sem);

        /* Simulate going to the toilet. */
        sleep((rand() % (MAXTIMEIN - MINTIMEIN)) + MINTIMEIN);

        sem_wait(&crit_sem);
        times_used++;
        males_inside--;
        printf("Male exiting bathroom.\n");
        print_status();
        if(males_inside == 0)
        {
            printf("Last male has exited bathroom.\n");
            print_status();
            if(females_waiting > 0)
                sem_post(&waiting_female_sem);
            else
                sem_post(&crit_sem);
        }
        else
            sem_post(&crit_sem);
    }
    pthread_exit(NULL);
}

void * female(void * input)
{
    int i;
    int females_in_queue;
    while(times_used < MAXTIMES)
    {
        /* We simulate time until we need to go to the bathroom. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL)) + MININTERVAL);

        sem_wait(&crit_sem);
        females_waiting++;
        printf("Female trying to enter bathroom.\n");
        print_status();

        if(males_inside > 0 || males_waiting > 0)
        {
            printf("Females waiting for their turn.\n");
            print_status();
            sem_post(&crit_sem);
            sem_wait(&waiting_female_sem); /* Waiting for our turn. */
            females_inside++; /* We use pass the baton technique. */
            females_waiting--;
            if(letting_in_people == false)
            {
                letting_in_people = true;
                females_in_queue = females_waiting;
                printf("Females turn, first one entering.\n");
                print_status();
                for(i = 0; i < females_in_queue; i++)
                {
                    sem_post(&waiting_female_sem);
                    sem_wait(&crit_sem);
                }
                letting_in_people = false;
            }
            else
            {
                printf("Female entering.\n");
                print_status();
            }
        }
        else /* Females are already inside. */
        {
            females_inside++;
            females_waiting--;
            printf("Female entering.\n");
            print_status();
        }
        sem_post(&crit_sem);

        /* Simulate going to the toilet. */
        sleep((rand() % (MAXTIMEIN - MINTIMEIN)) + MINTIMEIN);

        sem_wait(&crit_sem);
        times_used++;
        females_inside--;
        printf("Female exiting bathroom.\n");
        print_status();
        if(females_inside == 0)
        {
            printf("Last female has exited bathroom.\n");
            print_status();
            if(males_waiting > 0)
                sem_post(&waiting_male_sem);
            else
                sem_post(&crit_sem);
        }
        else
            sem_post(&crit_sem);
    }
    pthread_exit(NULL);
}
