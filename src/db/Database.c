#include <stdlib.h>
#include <string.h>

#include "Hash.h"
#include "Set.h"
#include "Database.h"
#include "ValueObject.h"
#include "Debug.h"

struct Database* newDB(unsigned int numOfDB)
{
    struct Database* pDB;
    pDB = (struct Database*)malloc(sizeof(struct Database));

    pDB->nDB = numOfDB;
    pDB->phash = (struct Hash**)malloc(sizeof(struct Hash*)*pDB->nDB);
    for (unsigned int i = 0; i < pDB->nDB; ++i)
    {
        pDB->phash[i] = newHash();
    }
    zmprintf("Creating database done, nDB = %u\n", pDB->nDB);

    return pDB;
}

void closeDB(struct Database* pDB)
{
    for (unsigned int i = 0; i < pDB->nDB; ++i)
    {
        freeHash(pDB->phash[i]);
    }
    free(pDB->phash);
    free(pDB);
    zmprintf("Closing database done.\n");
}

// 删除键值对，无检查
void removeMapFromDB(struct Database* pDB, const char* key, int index)
{
    struct ValueObject* pObj;
    pObj = (struct ValueObject*)getValue(pDB->phash[index], key);

    char typeStr[15];

    switch (pObj->type)
    {
        case VALUE_TYPE_STR:
            free(pObj->value.str);
            strcpy(typeStr, "string");
            break;
        
        case VALUE_TYPE_HASH:
            freeHash(pObj->value.pHash);
            strcpy(typeStr, "hash");
            break;
            
        case VALUE_TYPE_SET:
            freeSet(pObj->value.pSet);
            strcpy(typeStr, "set");
            break;
    }
    removeKey(pDB->phash[index], key);
    zmprintf("Removing map %s(%s) from DB done.\n", key, typeStr);
}

// 删除键值对，会先判断键是否存在
void removeMapFromDBIfExist(struct Database* pDB, const char* key, int index)
{
    // 检查键是否存在
    if (existKey(pDB->phash[index], key))
        removeMapFromDB(pDB, key, index);
}