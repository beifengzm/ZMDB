#pragma once

#include <stdlib.h>
#include <sys/time.h>

struct Timer
{
    long interval;  // 时间间隔(毫秒)
    long when;
    void* data;  // 处理器需要的数据
    void (*timeProc)(void* arg);
    void (*freeSelf)(struct Timer* pTimer);
};

struct PriorityQueue;
struct PriorityQueue* newTimerQueue();
long getNowTime();
long getExpireTime(long millionseconds);  // 计算millionseconds之后的时间
void addTimer(struct PriorityQueue* pQueue, struct Timer* pTimer);  
long nearestLeftTime(struct PriorityQueue* pQueue);  // 最近的时间事件剩余的时间
void removeTimer(struct PriorityQueue* pQueue, struct Timer* pTimer);
void runTimer(struct PriorityQueue* pQueue, struct Timer* pTimer);
void runTimerQueue(struct PriorityQueue* pQueue, int num);  // 随机选取定时器执行
void freeTimerQueue(struct PriorityQueue* pQueue);