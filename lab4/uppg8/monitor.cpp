#include "monitor.h"
/**
 * Intialize the monitor.
 */
monitor::monitor()
{
    /* Initiates the lock and the condition variables. */
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&man_cond, NULL);
    pthread_cond_init(&woman_cond, NULL);

    /* Initiates the counters. */
    men_inside = 0;
    men_waiting = 0;
    women_inside = 0;
    women_waiting = 0;
}

/** 
 * Destructor. Nothing to do here as there are nothing saved on the heap.
 */
monitor::~monitor()
{
}

/**
 * Method used for when a man wants to enter. Very similair to the woman version.
 *
 * Solution is fair because males are always going to enter. There is no way of
 * starvation. If there are no women waiting or inside we just enter the bathroom.
 * If there are woman waiting we stand in line and wait for a maximum of 2 turns
 * inside the bathroom. One if there are any remaining men inside the bathroom and
 * one turn for the women.
 */
void monitor::man_enter()
{
    pthread_mutex_lock(&lock); /* Lock to ensure mutual exclusion. */

    /* If we have to wait for our turn. Then we wait for the man condition variable.
     * This variable will be broadcasted to by when the last female exits the bathroom.*/
    if(women_inside > 0 || women_waiting > 0)
    {
        std::cout << "Man is trying to enter, but he has to wait." << std::endl;
        men_waiting++; /* Signal that we have a man waiting to get inside. */
        print_status(); /* Prints status of the current state of the bathroom and queue. */

        /* Surround the waiting around a do-while to be sure that we are safe to enter.
         * Reason that there is no check for women waiting here is that when woken up by the
         * last woman inside the bathroom there could be women waiting to get inside, therefore
         * that check would provide a deadlock. */
        do
        {
            pthread_cond_wait(&man_cond, &lock);
        } while(women_inside > 0);

        /* The man is now inside the bathroom. Update information to reflect this. */
        men_waiting--;
        men_inside++;
        std::cout << "Man is entering after waiting in line." << std::endl;
    }
    /* Enter straight away because there are no women inside or waiting. */
    else
    {
        men_inside++;
        std::cout << "Man entering without waiting." << std::endl;
    }
    print_status(); /* Prints status of the current state of the bathroom and queue. */
    pthread_mutex_unlock(&lock); /* Unlock again because we no longer need mutual exclusion. */
}

/**
 * Method used when a man exists the bathroom. Very similair to the woman version.
 *
 * Updates counters and then checks if we were the last one to exit, if we were we need to
 * signal to the women waiting to enter.
 */
void monitor::man_exit()
{
    pthread_mutex_lock(&lock); /* Lock to provide mutual exclusion. */
    men_inside--; /* Update counter. */
    std::cout << "Man exiting bathroom." << std::endl;
    /* If we were the last male to exit the bathroom we need to signal to any women
     * waiting to enter. */
    if(men_inside == 0)
    {
        /* Print information that we were the last male. */
        std::cout << "Last man has exited the bathroom." << std::endl;
        /* If there are women waiting we signal that they can enter */
        if(women_waiting > 0)
        {
            std::cout << "Signaling to the women to enter." << std::endl;
            pthread_cond_broadcast(&woman_cond);
        }
    }
    print_status(); /* Prints status of the current state of the bathroom and queue. */
    pthread_mutex_unlock(&lock); /* We no longer need mutual exclusion. */
}

/**
 * Method used for when a woman wants to enter. Very similair to the man version.
 *
 * Solution is fair because women are always going to be allowed to enter. There is no
 * way of starvation. If there are no men waiting or inside we just enter the bathroom.
 * If there are men waiting we stand in line and wait for a maximum of 2 turns
 * inside the bathroom; one if there are any remaining women inside the bathroom and
 * one turn for the men.
 */
void monitor::woman_enter()
{
    pthread_mutex_lock(&lock); /* Lock to provide mutual exclusion. */
    /* If we have to wait in line before entering the bathroom. Then we wait for the
     * woman condition variable. This condition variable will always be broadcasted to
     * by the last male to exit the bathroom. */
    if(men_inside > 0 || men_waiting > 0)
    {
        std::cout << "Woman is trying to enter, but she has to wait." << std::endl;
        women_waiting++; /* Update counter of women waiting. */
        print_status(); /* Prints status of the current state of the bathroom and queue. */
        /* To ensure that it really is safe for us to enter the bathroom we surround the
         * waiting with a do-while. In this loop we cannot check for women waiitng because
         * that would become a deadlock when if males have arrived after us to the queue and
         * want to enter as well. */
        do
        {
            pthread_cond_wait(&woman_cond, &lock);
        } while(men_inside > 0);
        /* Update counters to reflect that we are inside the bathroom. */
        women_waiting--;
        women_inside++;
        std::cout << "Woman is entering after waiting in line." << std::endl;
    }
    /* We are safe to enter straight away. */
    else
    {
        women_inside++; /* Update counter. */
        std::cout << "Woman entering without waiting." << std::endl;
    }
    print_status(); /* Prints status of the current state of the bathroom and queue. */
    pthread_mutex_unlock(&lock); /* Unlock before we exit. */
}

/**
 * Method used when a woman exists the bathroom. Very similair to the man version.
 *
 * Updates counters and then checks if we were the last one to exit, if we were we need to
 * signal to the men waiting to enter.
 */
void monitor::woman_exit()
{
    pthread_mutex_lock(&lock); /* Lock for mutual exclusion. */
    women_inside--; /* Update counter. */
    std::cout << "Woman exiting bathroom." << std::endl;
    if(women_inside == 0) /* If we were the last women to exit we need to signal to the men. */
    {
        std::cout << "Last woman has exited the bathroom." << std::endl;
        if(men_waiting > 0) /* If there are any men waiting for us to signal to. */
        {
            std::cout << "Signaling to the men to enter." << std::endl;
            pthread_cond_broadcast(&man_cond);
        }
    }
    print_status(); /* Prints status of the current state of the bathroom and queue. */
    pthread_mutex_unlock(&lock); /* Unlock before exiting method. */
}


/**
 * Prints the relevant status of the 2 queues and which gender is inside and how many
 * they are.
 */
void monitor::print_status()
{
    std::cout << std::endl << "Men inside = " << men_inside << ", women inside = " 
        << women_inside << std::endl << "Men waiting = " << men_waiting << 
        ", women waiting = " << women_waiting << std::endl << std::endl;
}
