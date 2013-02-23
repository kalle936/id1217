#ifndef __PPS_MONITOR
#define __PPS_MONITOR

#include <cstddef>
#include <iostream>
#include <pthread.h>

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
        void man_enter();
        void man_exit();
        void woman_enter();
        void woman_exit();
    private:
        /* Method for printing relevant information about the
         * persons inside the bathroom and the persons waiting
         * outside the bathroom. */
        void print_status();

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
