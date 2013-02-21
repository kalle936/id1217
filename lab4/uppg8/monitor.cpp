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
        std::cout << "Man is trying to enter, but he has to wait." << std::endl;
        men_waiting++;
        print_status();
        do
        {
            pthread_cond_wait(&man_cond, &lock);
        } while(women_inside > 0);
        men_waiting--;
        men_inside++;
        std::cout << "Man is entering after waiting in line." << std::endl;
    }
    else
    {
        men_inside++;
        std::cout << "Man entering without waiting." << std::endl;
    }
    print_status();
    pthread_mutex_unlock(&lock);
}

void monitor::man_exit()
{
    pthread_mutex_lock(&lock);
    men_inside--;
    std::cout << "Man exiting bathroom." << std::endl;
    if(men_inside == 0)
    {
        std::cout << "Last man has exited the bathroom." << std::endl;
        if(women_waiting > 0)
        {
            std::cout << "Signaling to the women to enter." << std::endl;
            pthread_cond_broadcast(&woman_cond);
        }
    }
    print_status();
    pthread_mutex_unlock(&lock);
}

void monitor::woman_enter()
{
    pthread_mutex_lock(&lock);
    if(men_inside > 0 || men_waiting > 0)
    {
        std::cout << "Woman is trying to enter, but she has to wait." << std::endl;
        women_waiting++;
        print_status();
        do
        {
            pthread_cond_wait(&woman_cond, &lock);
        } while(men_inside > 0);
        women_waiting--;
        women_inside++;
        std::cout << "Woman is entering after waiting in line." << std::endl;
    }
    else
    {
        women_inside++;
        std::cout << "Woman entering without waiting." << std::endl;
    }
    print_status();
    pthread_mutex_unlock(&lock);
}

void monitor::woman_exit()
{
    pthread_mutex_lock(&lock);
    women_inside--;
    std::cout << "Woman exiting bathroom." << std::endl;
    if(women_inside == 0)
    {
        std::cout << "Last woman has exited the bathroom." << std::endl;
        if(men_waiting > 0)
        {
            std::cout << "Signaling to the men to enter." << std::endl;
            pthread_cond_broadcast(&man_cond);
        }
    }
    print_status();
    pthread_mutex_unlock(&lock);
}

void monitor::print_status()
{
    std::cout << std::endl << "Men inside = " << men_inside << ", women inside = " 
        << women_inside << std::endl << "Men waiting = " << men_waiting << 
        ", women waiting = " << women_waiting << std::endl << std::endl;
}
