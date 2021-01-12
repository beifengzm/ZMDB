#pragma once

struct ListEntry
{
    void* value;
    struct ListEntry* last;
    struct ListEntry* next; 
};

struct LinkedList
{
    unsigned int size;
    struct ListEntry* pHead;
    struct ListEntry* pRear;
};

struct LinkedList* newLinkedList();