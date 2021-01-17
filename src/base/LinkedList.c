#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

struct ListEntry* newListEntry(void* value, nbyte_t nValue)
{
    struct ListEntry* pEntry;
    pEntry = (struct ListEntry*)malloc(sizeof(struct ListEntry));

    pEntry->value = malloc(nValue);
    memcpy(pEntry->value, value, nValue);
    pEntry->nValue = nValue;

    pEntry->last = pEntry->next = NULL;

    return pEntry;
}

struct LinkedList* newLinkedList()
{
    struct LinkedList* plist;
    plist = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    plist->size = 0;
    plist->phead = plist->prear = NULL;
    plist->iterStart = 0;

    return plist;
}

int getListSize(struct LinkedList* plist)
{
    return plist->size;
}

int emptyLinkedList(struct LinkedList* plist)
{
    return plist->size==0?1:0;
}

void lpush(struct LinkedList* plist, void* value, nbyte_t nValue)
{
    struct ListEntry* pEntry = newListEntry(value, nValue);
    plist->size++;
    if (plist->phead == NULL)
    {
        plist->phead = plist->prear = pEntry;
        return;
    }

    pEntry->next = plist->phead;
    plist->phead->last = pEntry;
    plist->phead = pEntry;
}

void rpush(struct LinkedList* plist, void* value, nbyte_t nValue)
{
    struct ListEntry* pEntry = newListEntry(value, nValue);
    plist->size++;
    if (plist->phead == NULL)
    {
        plist->phead = plist->prear = pEntry;
        return;
    }

    pEntry->last = plist->prear;
    plist->prear->next = pEntry;
    plist->prear = pEntry;
}

void* getValueByIndex(struct LinkedList* plist, int index)
{
    if (index == -1)
        return plist->prear->value;

    struct ListEntry* pcur = plist->phead;
    int k = index-1;
    while (pcur && k--)
        pcur = pcur->next;
    return pcur->value;
}

void removeFromLinkedList(struct LinkedList* plist, struct ListEntry* pEntry)
{
    if (pEntry->last)
        pEntry->last->next = pEntry->next;
    else // 删除节点为头结点 
        plist->phead = pEntry->next;
    if (pEntry->next)
        pEntry->next->last = pEntry->last;
    else 
        plist->prear = pEntry->last;
    freeListEntry(pEntry);
}

void* front(struct LinkedList* plist)
{
    return plist->phead->value;
}

void* back(struct LinkedList* plist)
{
    return plist->prear->value;
}

void lpop(struct LinkedList* plist)
{
    if (plist->size == 0)
        return;

    struct ListEntry* ptmp = plist->phead;
    plist->phead = plist->phead->next;
    if (plist->phead)
        plist->phead->last = NULL;
    else
        plist->prear = NULL;
    freeListEntry(ptmp);
    plist->size--;
}

void rpop(struct LinkedList* plist)
{
    if (plist->size == 0)
        return;

    struct ListEntry* ptmp = plist->prear;
    plist->prear = plist->prear->last;
    if (plist->prear)
        plist->prear->next = NULL;
    else
        plist->phead = NULL;
    freeListEntry(ptmp);
    plist->size--;
}

int iterateList(struct LinkedList* plist, void** presult)
{
    if (!plist->iterStart)
    {
        plist->iter = plist->phead;
        plist->iterStart = 1;
    }
    if (plist->iter)
    {
        *presult = plist->iter->value;
        plist->iter = plist->iter->next;
        return 1;
    }
    else
    {
        plist->iterStart = 0;
        return 0;
    }
}

int rangeList(struct LinkedList* plist, int start, int end, void** presult)
{
    if (!plist->iterStart)
    {
        plist->iter = plist->phead;
        int k = start-1;
        while (k--) 
            plist->iter = plist->iter->next;
        plist->iterStart = 1;
        if (end == -1) end = plist->size;
        plist->iterCount = end-start+1;
    }
    if (plist->iterCount)
    {
        *presult = plist->iter->value;
        plist->iter = plist->iter->next;
        plist->iterCount--;
        return 1;
    }
    else
    {
        plist->iterStart = 0;
        return 0;
    }
}

void freeListEntry(struct ListEntry* pEntry)
{
    free(pEntry->value);
    free(pEntry);
}

void freeLinkedList(struct LinkedList* plist)
{
    struct ListEntry *pcur = plist->phead, *pnext;
    while (pcur)
    {
        pnext = pcur->next;
        freeListEntry(pcur);
        pcur = pnext;
    }
    free(plist);
}
