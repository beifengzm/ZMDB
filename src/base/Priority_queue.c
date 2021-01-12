#include <stdlib.h>
#include <string.h>

#include "Priority_queue.h"
#include "Debug.h"

struct PriorityQueue* newPriorityQueue(Comparator cmp)
{
    struct PriorityQueue* pQueue;
    pQueue = (struct PriorityQueue*)malloc(sizeof(struct PriorityQueue));

    pQueue->cmp = cmp;
    pQueue->capcity = 4;
    pQueue->size = 0;
    pQueue->pArray = (void**)malloc(sizeof(void*)*pQueue->capcity);

    return pQueue;
}

// 上滤
static void upQueue(struct PriorityQueue* pQueue, int p)
{
    int parent;
    while (p > 0)
    {
        parent = (p-1)/2;
        if (pQueue->cmp(pQueue->pArray[p], pQueue->pArray[parent]))
        {
            void* tmp = pQueue->pArray[p];
            pQueue->pArray[p] = pQueue->pArray[parent];
            pQueue->pArray[parent] = tmp;
        } 
        else
            break;
        p = parent;
    }
}

//添加元素
void addElement(struct PriorityQueue* pQueue, void* pElement)
{
    if (pQueue->size == pQueue->capcity)
    {
        void** oldArray = pQueue->pArray;
        pQueue->capcity *= 2;
        pQueue->pArray = (void**)malloc(sizeof(void*)*pQueue->capcity);
        memcpy(pQueue->pArray, oldArray, sizeof(void*)*pQueue->size);
        free(oldArray);
    }

    pQueue->pArray[pQueue->size++] = pElement;
    upQueue(pQueue, pQueue->size-1);
}

int emptyQueue(struct PriorityQueue* pQueue)
{
    return pQueue->size>0?0:1;
}

void* topQueue(struct PriorityQueue* pQueue)
{
    if (emptyQueue(pQueue))
        return (void*)NULL;
    return pQueue->pArray[0];
}

void removeByIndex(struct PriorityQueue* pQueue, int p)
{
    while (p < pQueue->size)
    {
        int selectedcChild = -1;
        int left = p*2+1, right = p*2+2;
        if (left < pQueue->size) // 左孩子存在
            selectedcChild = left;
        if (right < pQueue->size && 
        pQueue->cmp(pQueue->pArray[right], pQueue->pArray[left])) // 右孩子也存在
            selectedcChild = right;
        if (selectedcChild != -1)  // 非叶子节点
        {
            pQueue->pArray[p] = pQueue->pArray[selectedcChild];
            p = selectedcChild;
        }
        else 
        {
            pQueue->pArray[p] = pQueue->pArray[pQueue->size-1];
            upQueue(pQueue, p);
            break;
        }
    }
    pQueue->size--;

    // shrink
    if (pQueue->capcity>4 && pQueue->size*2<pQueue->capcity)
    {
        void** oldArray = pQueue->pArray;
        pQueue->capcity /= 2;
        pQueue->pArray = (void**)malloc(sizeof(void*)*pQueue->capcity);
        memcpy(pQueue->pArray, oldArray, sizeof(void*)*pQueue->size);
        free(oldArray);
    }
}

void removeElement(struct PriorityQueue* pQueue, void* ptr)
{
    int index = pQueue->size;
    for (int i = 0; i < pQueue->size && index == pQueue->size; ++i)
        if (ptr == pQueue->pArray[i]) 
            index = i;
    if (index != pQueue->size)
        removeByIndex(pQueue, index);
}

void popQueue(struct PriorityQueue* pQueue)
{
    if (!emptyQueue(pQueue))
        removeByIndex(pQueue, 0);
}

void freeQueue(struct PriorityQueue* pQueue)
{
    free(pQueue);
}
