#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

struct Timer
{
    long interval;  // 时间间隔(毫秒)
    long when;      // millionsecond
};

int main()
{
    struct Timer timer;
    timer.interval = 10000;
    timer.when = 0;
    for (;;)
    {
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        if (tv_now.tv_sec*1000+tv_now.tv_usec/1000 >= timer.when)
        {
            printf("time event: %ld\n", timer.when);
            timer.when = tv_now.tv_sec*1000+tv_now.tv_usec/1000+timer.interval;
        }
    }

    return 0;
}