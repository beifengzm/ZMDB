#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ValueObject.h"
#include "LinkedList.h"
#include "Timer.h"
#include "Server.h"
#include "Database.h"
#include "Client.h"
#include "Hash.h"
#include "Set.h"

// 删除旧的键值对(如果存在)
void removeOldMapIfExist(struct DBServer* pServer, struct DBClient* pClient)
{
    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);

    // 如果该键存在，则删除
    if (pObj && pObj->type != VALUE_TYPE_SET)
    {
        if (pObj->pTimer)
            removeTimer(pServer->pTimerQueue, pObj->pTimer);
        removeMapFromDB(pServer->pDB, pClient->argv[1], pClient->db_index);
    }
}

// 检查键是否存在或者过期
struct ValueObject* getObjectAndCheck(struct DBServer* pServer,
         struct DBClient* pClient)
{
    struct ValueObject* pObj;

    pObj = (struct ValueObject*)getValue(pServer->pDB->phash[pClient->db_index], 
            pClient->argv[1]);

    // 检查键是否存在
    if (!pObj)
    {
        sprintf(pClient->sendBuff, "No corresponding key: %s", pClient->argv[1]);
        return NULL;
    }

    if (pObj->pTimer==NULL) 
        return pObj;

    // 检查键是否过期
    if (getNowTime() >= pObj->pTimer->when)
    {
        runTimer(pServer->pTimerQueue, pObj->pTimer);
        sprintf(pClient->sendBuff, "No corresponding key: %s", pClient->argv[1]);
        return NULL;
    }

    return pObj;
}

// 检查键是否存在或者过期
void* getTypeObjectAndCheck(struct DBServer* pServer,
         struct DBClient* pClient, int type)
{
    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);

    pObj = (struct ValueObject*)getValue(pServer->pDB->phash[pClient->db_index], 
            pClient->argv[1]);

    // 检查键是否存在
    if (!pObj)
        return NULL;

    // 检查类型是否匹配
    if (type != pObj->type)
    {
        sprintf(pClient->sendBuff, "(error) Incompatible type");
        return NULL;
    }

    switch (pObj->type)
    {
        case VALUE_TYPE_STR:
            return (void*)pObj->value.str;

        case VALUE_TYPE_HASH:
            return (void*)pObj->value.pHash;

        case VALUE_TYPE_SET:
            return (void*)pObj->value.pSet;

        case VALUE_TYPE_LIST:
            return (void*)pObj->value.plist;
        
        default:
            return (void*)NULL;
    }

    return (void*)NULL;
}

// 获取值对象，如果键值对不存在，则新建
struct ValueObject* getObjectAndCreate(struct DBServer* pServer,
         struct DBClient* pClient, int type)
{
    if (!existKey(pServer->pDB->phash[pClient->db_index], 
                pClient->argv[1]))  // 该键不存在，则新建
    {
        struct ValueObject object;
        object.type = type;
        object.pTimer = NULL;
        switch (type)
        {
            case VALUE_TYPE_STR:
                object.value.str = (char*)malloc(strlen(pClient->argv[2])+1);
                break;
            
            case VALUE_TYPE_HASH:
                object.value.pHash = newHash();
                break;

            case VALUE_TYPE_SET:
                object.value.pSet = newSet();
                break;

            case VALUE_TYPE_LIST:
                object.value.plist = newLinkedList();
                break;
            
            default:
                break;
        }
        putKV(pServer->pDB->phash[pClient->db_index], pClient->argv[1], 
                    &object, sizeof(struct ValueObject));
    }
    return (struct ValueObject*)getValue(pServer->pDB->phash[pClient->db_index], 
        pClient->argv[1]);
}