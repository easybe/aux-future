#include <sys/types.h>
#include <sys/time.h>

/*
 * Both clocks return the time of day. A/UX has no monotonic clock.
 */
int clock_gettime(int clock_id, struct timespec *ts)
{
    struct timeval tv;

    (void)clock_id;
    if (gettimeofday(&tv, NULL) != 0)
        return -1;
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = tv.tv_usec * 1000L;
    return 0;
}
