#pragma once

typedef unsigned int nbyte_t;

struct ListEntry
{
    void* value;
    nbyte_t nValue;
    struct ListEntry* last;
    struct ListEntry* next; 
};

struct LinkedList
{
    unsigned int size;
    struct ListEntry* phead;
    struct ListEntry* prear;

    struct ListEntry* iter;
    int iterStart, iterCount;
};

struct ListEntry* newListEntry(void* value, nbyte_t nValue);
struct LinkedList* newLinkedList();
int getListSize(struct LinkedList* plist);
int emptyLinkedList(struct LinkedList* plist);
void lpush(struct LinkedList* plist, void* value, nbyte_t nValue);
void rpush(struct LinkedList* plist, void* value, nbyte_t nValue);
void* getValueByIndex(struct LinkedList* plist, int index);
void removeFromLinkedList(struct LinkedList* plist, struct ListEntry* pEntry);
void* front(struct LinkedList* plist);
void* back(struct LinkedList* plist);
void lpop(struct LinkedList* plist);
void rpop(struct LinkedList* plist);
int iterateList(struct LinkedList* plist, void** presult);
int rangeList(struct LinkedList* plist, int start, int end, void** presult);
void freeListEntry(struct ListEntry* pEntry);
void freeLinkedList(struct LinkedList* plist);