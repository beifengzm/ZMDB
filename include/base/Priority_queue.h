#pragma once

struct PriorityQueue
{
    void** pArray;
    unsigned int size;
    unsigned int capcity;
    int (*cmp)(void*, void*);
};

// 小顶堆: <(1),other(0); 大顶堆: >(1),other(0)
typedef int (*Comparator)(void*, void*);  
struct PriorityQueue* newPriorityQueue(Comparator cmp);
void addElement(struct PriorityQueue* pQueue, void* pElement);
void removeElement(struct PriorityQueue* pQueue, void* ptr);
int emptyQueue(struct PriorityQueue* pQueue);
void* topQueue(struct PriorityQueue* pQueue);
void popQueue(struct PriorityQueue* pQueue);
void freeQueue(struct PriorityQueue* pQueue);