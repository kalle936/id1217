#include "monitor.h"

monitor::monitor()
{
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&man_cond, NULL);
    pthread_cond_init(&woman_cond, NULL);

    men_inside = 0;
    men_waiting = 0;
    women_inside = 0;
    women_waiting = 0;
}

monitor::~monitor()
{
}

monitor::monitor(const monitor & other)
{
}

monitor & monitor::operator=(const monitor & source)
{
    return *this;
}

void monitor::man_enter()
{
    pthread_mutex_lock(&lock);
    if(women_inside > 0 || women_waiting > 0)
    {
        men_waiting++;
        while(women_inside > 0)
        {
            pthread_cond_wait(&man_cond, &lock);
        }
        men_waiting--;
        men_inside++;
    }
    else
    {
        men_inside++;
    }
    pthread_mutex_unlock(&lock);
}

void monitor::man_exit()
{
    pthread_mutex_lock(&lock);
    men_inside--;
    if(men_inside == 0)
    {
        if(women_waiting > 0)
        {
            pthread_cond_broadcast(&woman_cond);
        }
    }
    pthread_mutex_unlock(&lock);
}

void monitor::woman_enter()
{
    pthread_mutex_lock(&lock);
    if(men_inside > 0 || men_waiting > 0)
    {
        women_waiting++;
        while(men_inside > 0)
        {
            pthread_cond_wait(&woman_cond, &lock);
        }
        women_waiting--;
        women_inside++;
    }
    else
    {
        women_inside++;
    }
    pthread_mutex_unlock(&lock);
}

void monitor::woman_exit()
{
    pthread_mutex_lock(&lock);
    women_inside--;
    if(women_inside == 0)
    {
        if(men_waiting > 0)
        {
            pthread_cond_broadcast(&man_cond);
        }
    }
    pthread_mutex_unlock(&lock);
}
