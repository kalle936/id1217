#ifndef __PPS_MONITOR
#define __PPS_MONITOR

#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

/**
 * Monitor implementation for the unisex bathroom.
 */
class monitor
{
    public:
        monitor(); /* Constructor. */
        ~monitor(); /* Destructor. */

        /* Methods for controling how men and women enter
         * and exits the bathroom. */
        void man_enter(long number);
        void man_exit(long number);
        void woman_enter(long number);
        void woman_exit(long number);
    private:
        /* Method for printing relevant information about the
         * persons inside the bathroom and the persons waiting
         * outside the bathroom. */
        void print_status();

        /* Reads the timer to provide a timestamp for the printing. */
        double read_timer();

        /* Conditional variables, one for men entering and one
         * for women entering. */
        pthread_cond_t man_cond;
        pthread_cond_t woman_cond;

        /* Lock to provide mutual exclusion. */
        pthread_mutex_t lock;

        /* Counters. */
        int men_inside;
        int men_waiting;
        int women_inside;
        int women_waiting;
};
#endif
