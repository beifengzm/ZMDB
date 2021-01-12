#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"

static int putEntry(struct Hash* phash, struct HashEntry* pEntry);
static unsigned int getIndex(struct Hash* phash, const void* key);
static void expand(struct Hash* phash);
static void shrink(struct Hash* phash);
static void freeHashEntry(struct HashEntry* pEntry);

static struct HashEntry* newEntry(const void* key, const void* value, nbyte_t nValue)
{
    int nKey = strlen(key)+1;
    struct HashEntry* pEntry;
    pEntry = (struct HashEntry*)malloc(sizeof(struct HashEntry));
    pEntry->key = (void*)malloc(nKey);
    memcpy(pEntry->key, key, nKey);
    pEntry->nKey = nKey;
    pEntry->value = (void*)malloc(nValue);
    memcpy(pEntry->value, value, nValue);
    pEntry->nValue = nValue;
    return pEntry;
}

struct Hash* newHash()
{
    struct Hash* phash;
    phash = (struct Hash*)malloc(sizeof(struct Hash));
    phash->capcity = 4;   // 起始桶容量为4
    phash->size = 0;
    phash->iterStart = 0;
    phash->pHead = phash->pRear = NULL;
    phash->hashTable = (struct HashEntry**)malloc(sizeof(struct HashEntry*)*phash->capcity);
    memset(phash->hashTable, 0, sizeof(struct HashEntry*)*phash->capcity);
    return phash;
}

static unsigned int getIndex(struct Hash* phash, const void* key)
{
    unsigned int hashCode = 0;
    int nbyte = strlen(key)+1;
    unsigned char* pbase = (unsigned char*)key;
    if (nbyte > 10000) nbyte = 10000;
    for (unsigned int i = 0; i < nbyte; ++i)
    {
        hashCode = hashCode*31 + *pbase++;
    }
    return hashCode&(phash->capcity-1);
}

int putKV(struct Hash* phash, const void* key, const void* value, nbyte_t nValue)
{
    if (phash->size/(double)phash->capcity >= 0.72)   // 装填因子大于等于0.75时扩充
    {
        expand(phash);
    }   

    int nKey = strlen(key)+1;
    struct HashEntry* pEntry = newEntry(key, value, nValue);
    int isSuccessed = putEntry(phash, pEntry);
    if (isSuccessed)
    {
        phash->size++;
    }
    return isSuccessed;
}

int setValue(struct Hash* phash, const void* key, const void* value, nbyte_t nValue)
{
    int nKey = strlen(key)+1;
    unsigned int index = getIndex(phash, key);
    struct HashEntry* pEntry = newEntry(key, value, nValue);

    struct HashEntry* tmpEntry = phash->hashTable[index];
    while (tmpEntry != NULL)
    {
        if (!memcmp(key, tmpEntry->key, nKey))
        {
            tmpEntry->value = (void*)realloc(tmpEntry->value, pEntry->nValue);
            memcpy(tmpEntry->value, pEntry->value, pEntry->nValue);
            tmpEntry->nValue = pEntry->nValue;
            freeHashEntry(pEntry);
            return 1;
        }
        tmpEntry = tmpEntry->next;
    }
    freeHashEntry(pEntry);
    return 0;
}

int sizeHash(struct Hash* phash)
{
    return phash->size;
}

int capcityHash(struct Hash* phash)
{
    return phash->capcity;
}

int removeKey(struct Hash* phash, const void* key)
{
    int nKey = strlen(key)+1;
    unsigned int index = getIndex(phash, key);
    struct HashEntry* tmpEntry = phash->hashTable[index];
    struct HashEntry* pLastEntry = tmpEntry, *pNextEntry;
    int isSuccessed = 0;
    while (tmpEntry != NULL)
    {
        pNextEntry = tmpEntry->next;
        // 当前key已存在
        if (!memcmp(key, tmpEntry->key, nKey))
        {
            if (tmpEntry == phash->hashTable[index])
                phash->hashTable[index] = tmpEntry->next;
            else 
                pLastEntry->next = pNextEntry;

            if (tmpEntry->listlast)
                tmpEntry->listlast->listnext = tmpEntry->listnext;
            else  // 删除节点为头结点
                phash->pHead = phash->pHead->listnext;
            if (tmpEntry->listnext)
                tmpEntry->listnext->listlast = tmpEntry->listlast;
            else  // 删除节点为尾节点
                phash->pRear = phash->pRear->listlast;

            freeHashEntry(tmpEntry);
            phash->size--;
            isSuccessed = 1;
            break;
        }
        pLastEntry = tmpEntry;
        tmpEntry = pNextEntry;
    }
    if (phash->size>0 && phash->size/(double)phash->capcity <= 0.25)
    {
        shrink(phash);
    }
    return isSuccessed;
}

void* getValue(struct Hash* phash, const void* key)
{
    int nKey = strlen(key)+1;
    unsigned int index = getIndex(phash, key);
    struct HashEntry* tmpEntry = phash->hashTable[index];
    while (tmpEntry != NULL)
    {
        if (!memcmp(key, tmpEntry->key, nKey))
        {
            return tmpEntry->value;
        }
        tmpEntry = tmpEntry->next;
    }
    return (void*)NULL;
}

int existKey(struct Hash* phash, const void* key)
{
    return getValue(phash, key)==NULL?0:1;
}

unsigned int getSize(struct Hash* phash)
{
    return phash->size;
}

static void expand(struct Hash* phash)
{
    unsigned int oldCapcity = phash->capcity;
    struct HashEntry** oldHashTable = phash->hashTable;
    phash->pHead = phash->pRear = NULL;
    phash->capcity = oldCapcity << 1;
    phash->hashTable = (struct HashEntry**)malloc(sizeof(struct HashEntry*)*phash->capcity);
    memset(phash->hashTable, 0, sizeof(struct HashEntry*)*phash->capcity);
    for (unsigned int i = 0; i < oldCapcity; ++i)
    {
        struct HashEntry* pEntry = oldHashTable[i], *pNextEntry;
        while (pEntry != NULL)
        {
            pNextEntry = pEntry->next;
            putEntry(phash, pEntry);
            pEntry = pNextEntry;
        }
    }
    free(oldHashTable);
}

static void shrink(struct Hash* phash)
{
    unsigned int oldCapcity = phash->capcity;
    struct HashEntry** oldHashTable = phash->hashTable;
    phash->pHead = phash->pRear = NULL;
    phash->capcity = oldCapcity >> 1;
    phash->hashTable = (struct HashEntry**)malloc(sizeof(struct HashEntry*)*phash->capcity);
    memset(phash->hashTable, 0, sizeof(struct HashEntry*)*phash->capcity);
    for (unsigned int i = 0; i < oldCapcity; ++i)
    {
        struct HashEntry* pEntry = oldHashTable[i], *pNextEntry;
        while (pEntry != NULL)
        {
            pNextEntry = pEntry->next;
            putEntry(phash, pEntry);
            pEntry = pNextEntry;
        }
    }
    free(oldHashTable);
}

static int putEntry(struct Hash* phash, struct HashEntry* pEntry)
{
    pEntry->listlast = pEntry->listnext = NULL;
    unsigned int index = getIndex(phash, pEntry->key);
    struct HashEntry* tmpEntry = phash->hashTable[index];
    while (tmpEntry != NULL)
    {
        // 当前key已存在
        if (!memcmp(pEntry->key, tmpEntry->key, pEntry->nKey))
        {
            tmpEntry->value = (void*)realloc(tmpEntry->value, pEntry->nValue);
            memcpy(tmpEntry->value, pEntry->value, pEntry->nValue);
            tmpEntry->nValue = pEntry->nValue;
            freeHashEntry(pEntry);
            return 0;
        }
        tmpEntry = tmpEntry->next;
    }

    // 插入entry成功
    pEntry->next = phash->hashTable[index];
    phash->hashTable[index] = pEntry;

    if (phash->pHead == NULL)
    {
        phash->pHead = phash->pRear = pEntry;
    }
    else
    {
        phash->pRear->listnext = pEntry;
        pEntry->listlast = phash->pRear;
        phash->pRear = pEntry;
    }

    return 1;
}

static void freeHashEntry(struct HashEntry* pEntry)
{
    free(pEntry->key);
    free(pEntry->value);
    free(pEntry);
}

void freeHash(struct Hash* phash)
{
    struct HashEntry* iter, *iternext;
    for (iter = phash->pHead; iter != NULL;)
    {
        iternext = iter->listnext;
        freeHashEntry(iter);
        iter = iternext;
    }
    free(phash->hashTable);
    free(phash);
}

int iterateHash(struct Hash* phash, void** key, void** value)
{
    if (!phash->iterStart)
    {
        phash->iter = phash->pHead;
        phash->iterStart = 1;
    }
    if (phash->iter)
    {
        *key = phash->iter->key;
        *value = phash->iter->value;
        phash->iter = phash->iter->listnext;
        return 1;
    }
    else
    {
        phash->iterStart = 0;
        return 0;
    }
}