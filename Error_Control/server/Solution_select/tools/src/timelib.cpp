//
// Created by 229077035 on 2021/11/21.
//

/* -------------------------------------------------------------------------- */
/* --- Calculate Run-time ---------------------- */
#include <timelib.h>

double difftimespec(struct timespec end, struct timespec beginning) {
    double x;

    x = 1E-9 * (double) (end.tv_nsec - beginning.tv_nsec);
    x += (double) (end.tv_sec - beginning.tv_sec);

    return x;
}

void anotherStartTime(struct timespec *start, struct timespec *interv, struct timespec *anotherstart) {
    if ((start->tv_nsec - interv->tv_nsec) < 0) {
        anotherstart->tv_sec = start->tv_sec - interv->tv_sec - 1;
        anotherstart->tv_nsec = NANOSECOND + start->tv_nsec - interv->tv_nsec;
    } else {
        anotherstart->tv_sec = start->tv_sec - interv->tv_sec;
        anotherstart->tv_nsec = start->tv_nsec - interv->tv_nsec;
    }
    return;
}