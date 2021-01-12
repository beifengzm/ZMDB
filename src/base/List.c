#include <stdlib.h>

#include "List.h"

struct LinkedList* newLinkedList()
{
    struct LinkedList* pList;
    pList = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    pList->size = 0;
    pList->pHead = pList->pRear = NULL;

    return pList;
}

