#ifndef __PPS_MONITOR
#define __PPS_MONITOR

#include <cstddef>
#include <pthread.h>

class monitor
{
    public:
        monitor();
        ~monitor();
        monitor(const monitor & other);

        monitor & operator=(const monitor & other);

        const static int MAX_TIMES = 50;

        void man_enter();
        void man_exit();
        void woman_enter();
        void woman_exit();

    private:
        pthread_cond_t man_cond;
        pthread_cond_t woman_cond;
        pthread_mutex_t lock;
        int men_inside;
        int men_waiting;
        int women_inside;
        int women_waiting;
};
#endif
