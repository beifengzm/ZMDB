#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Set.h"
#include "Hash.h"

struct Set* newSet()
{
    struct Set* pSet;
    pSet = (struct Set*)malloc(sizeof(struct Set));
    pSet->pHash = newHash();

    return pSet;
}

int addSet(struct Set* pSet, const char* key)
{
    return putKV(pSet->pHash, key, (void*)NULL, 0);
}

int sizeSet(struct Set* pSet)
{
    return sizeHash(pSet->pHash);
}

int removeSet(struct Set* pSet, const char* key)
{
    return removeKey(pSet->pHash, key);
}

int sismember(struct Set* pSet, const char* key)
{
    return existKey(pSet->pHash, key);
}

void formatSet(struct Set* pSet, char* str)
{
    char* result;
    sprintf(str, "(set)\n");
    if (sizeSet(pSet) == 0) return;
    int i = 0;
    char buff[512], *p = str+strlen(str);
    while (iterateSet(pSet, (void**)&result))
    {
        sprintf(buff, "\n%d) %s"+!i, i+1, result);
        sprintf(p, "%s", buff);
        p += strlen(buff);
        i++;
    }
}

// 求两个集合的差集(result = set1 - set2)
struct Set* diffSet(struct Set* pSet1, struct Set* pSet2)
{
    struct Set* pSet = newSet();
    char* key;
    while (iterateSet(pSet1, (void**)&key))
    {
        if (!sismember(pSet2, key))
            addSet(pSet, key);
    }

    return pSet;
}

// 求两个集合的交集
struct Set* interSet(struct Set* pSet1, struct Set* pSet2)
{
    // 用较少元素的集合在较多元素的集合中查找
    if (pSet1->pHash->size > pSet2->pHash->size)
    {
        struct Set* tmpSet = pSet1;
        pSet1 = pSet2;
        pSet2 = tmpSet;
    }

    struct Set* pSet = newSet();
    char* key;
    while (iterateSet(pSet1, (void**)&key))
    {
        if (sismember(pSet2, key))
            addSet(pSet, key);
    }

    return pSet;
}

// 求多个集合的交集
struct Set* interSets(struct Set** pSets, int nSets)
{
    struct Set* tmpSet1 = pSets[0], *tmpSet2;
    for (int i = 1; i < nSets; ++i)
    {
        tmpSet2 = interSet(tmpSet1, pSets[i]);
        if (tmpSet1 != pSets[0])
            freeSet(tmpSet1);
        tmpSet1 = tmpSet2;
    }
    return tmpSet2;
}

// 求两个集合的并集
struct Set* unionSet(struct Set* pSet1, struct Set* pSet2)
{
    struct Set* pSet = newSet();
    char* key;
    while (iterateSet(pSet1, (void**)&key))
    {
        addSet(pSet, key);
    }
    while (iterateSet(pSet2, (void**)&key))
    {
        addSet(pSet, key);
    }
    return pSet;
}

struct Set* unionSets(struct Set** pSets, int nSets)
{
    struct Set* pSet = newSet();
    char* key;
    for (int i = 0; i < nSets; ++i)
    {
        while (iterateSet(pSets[i], (void**)&key))
            addSet(pSet, key);
    }
    return pSet;
}

int iterateSet(struct Set* pSet, void** pElement)
{
    if (!pSet->pHash->iterStart)
    {
        pSet->pHash->iter = pSet->pHash->pHead;
        pSet->pHash->iterStart = 1;
    }
    if (pSet->pHash->iter)
    {
        *pElement = pSet->pHash->iter->key;
        pSet->pHash->iter = pSet->pHash->iter->listnext;
        return 1;
    }
    else
    {
        pSet->pHash->iterStart = 0;
        return 0;
    }
}

void freeSet(struct Set* pSet)
{
    freeHash(pSet->pHash);
    free(pSet);
}
