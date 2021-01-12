#include <stdlib.h>
#include <time.h>

#include "Priority_queue.h"
#include "ObjectTimer.h"
#include "Debug.h"
#include "Timer.h"

int timeComparator(void* p1, void* p2)
{
    struct Timer *pTimer1, *pTimer2;
    pTimer1 = (struct Timer*)p1, pTimer2 = (struct Timer*)p2;
    return pTimer1->when < pTimer2->when;
}

long getNowTime()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    long now = tv_now.tv_sec*1000+tv_now.tv_usec/1000;
    return now;
}

long getExpireTime(long millionseconds)
{
    long when = getNowTime()+millionseconds;
    return when;
}

struct PriorityQueue* newTimerQueue()
{
    struct PriorityQueue* pQueue;
    pQueue = newPriorityQueue(timeComparator);
    return pQueue;
}

void addTimer(struct PriorityQueue* pQueue, struct Timer* pTimer)
{
   addElement(pQueue, pTimer);
}

long nearestLeftTime(struct PriorityQueue* pQueue)
{
    if (emptyQueue(pQueue)) return -1;
    struct Timer* pTimer = topQueue(pQueue);
    long left = pTimer->when-getNowTime();
    left = left <= 0?0:left;
    return left;
}

void removeTimer(struct PriorityQueue* pQueue, struct Timer* pTimer)
{
    if (pTimer)
    {
        removeElement(pQueue, pTimer);
        pTimer->freeSelf(pTimer);
    }
}

void runTimer(struct PriorityQueue* pQueue, struct Timer* pTimer)
{
    pTimer->timeProc(pTimer->data);
    removeTimer(pQueue, pTimer);
}

void runTimerQueue(struct PriorityQueue* pQueue, int num)
{
    long now = getNowTime();
    while (!emptyQueue(pQueue) && num--)
    {
        struct Timer* pTimer = (struct Timer*)topQueue(pQueue);
        if (now >= pTimer->when)
        {
            pTimer->timeProc(pTimer->data);
            popQueue(pQueue);
            pTimer->freeSelf(pTimer);
        }
        else 
            break;
    }
}

void freeTimerQueue(struct PriorityQueue* pQueue)
{
    freeQueue(pQueue);
}