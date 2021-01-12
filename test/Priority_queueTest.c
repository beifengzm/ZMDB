#include <stdio.h>

#include "Priority_queue.h"

int cmp(void* p1, void* p2)
{
    int* pInt1 = (int*)p1, *pInt2 = (int*)p2;
    return (*pInt1) < (*pInt2);
}

int main()
{
    int A[9] = {3,1,2,8,6,5,1,7,6};
    struct PriorityQueue* pQueue = newPriorityQueue(cmp);
    for (int i = 0; i < 9; ++i)
        addElement(pQueue, &A[i]);
    while (!emptyQueue(pQueue))
    {
        int* p = topQueue(pQueue);
        printf("top = %d, size: %u, capcity: %u\n", *p, pQueue->size, pQueue->capcity);
        popQueue(pQueue);
    }
    freeQueue(pQueue);

    return 0;
}