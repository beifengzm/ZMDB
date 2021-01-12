#include <stdio.h>
#include <stdlib.h>

#include "SetCommand.h"
#include "CCommand.h"
#include "Database.h"
#include "Hash.h"
#include "Set.h"
#include "Server.h"
#include "Client.h"
#include "Hash.h"
#include "ValueObject.h"

// 判断建是否存在，是否过期，是否是一个集合
struct Set* getSetAndCheck(struct DBServer* pServer, struct DBClient* pClient)
{
    struct ValueObject* pOBj = getObjectAndCheck(pServer, pClient);
    if (!pOBj) return NULL;
    if (pOBj->type != VALUE_TYPE_SET)
    {
        sprintf(pClient->sendBuff, "(error) %s is not a set.", pClient->argv[1]);
        return NULL;
    }
    return pOBj->value.pSet;
}

void saddCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SADD KEY V1 [V2 ...]");
        return;
    }

    int nSuccessed = 0;
    if (existKey(pServer->pDB->phash[pClient->db_index], 
                pClient->argv[1]))  // 该键存在
    {
        struct ValueObject* pObj;
        pObj = (struct ValueObject*)getValue(pServer->pDB->phash[pClient->db_index], 
            pClient->argv[1]);
        for (int i = 2; i < pClient->argc; ++i)
            nSuccessed += addSet(pObj->value.pSet, pClient->argv[i]);
    }
    else  // 该键不存在,则新建
    {
        struct ValueObject obj;
        initValueObject(&obj, pClient, VALUE_TYPE_SET);

        nSuccessed += putKV(pServer->pDB->phash[pClient->db_index], pClient->argv[1], 
                &obj, sizeof(struct ValueObject));
    }
    pServer->changed++;
    sprintf(pClient->sendBuff, "[OK] %d", nSuccessed);
}

void scardCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SCARD KEY");
        return;
    }

    struct ValueObject* pObj;
    pObj = (struct ValueObject*)getValue(
        pServer->pDB->phash[pClient->db_index], pClient->argv[1]);
    if (!pObj || pObj->type != VALUE_TYPE_SET)
    {
        sprintf(pClient->sendBuff, "(error) %s is not a set.", pClient->argv[1]);
        return;
    }

    struct Set* pSet = getSetAndCheck(pServer, pClient);
    if (pSet)
        sprintf(pClient->sendBuff, "(size) %d", sizeSet(pObj->value.pSet));
}

void sdiffCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SDIFF SET1 SET2");
        return;
    }

    struct Set* pSets[2];
    struct Set* pRetSet = NULL;

    int flag = 1;
    for (int i = 1; i < pClient->argc; ++i)
    {
        struct ValueObject* pObj;
        pObj = (struct ValueObject*)getValue(
            pServer->pDB->phash[pClient->db_index], pClient->argv[i]);
        if (!pObj || pObj->type != VALUE_TYPE_SET)
        {
            sprintf(pClient->sendBuff, "(error) %s is not a set.", pClient->argv[i]);
            flag = 0;
            break;
        }
        pSets[i-1] = pObj->value.pSet;
    }
    if (flag)
    {
        pRetSet = diffSet(pSets[0], pSets[1]);
        formatSet(pRetSet, pClient->sendBuff);
    }

    if (pRetSet) 
        free(pRetSet);
}

void sismemberCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SISMEMBER SET MEMBER");
        return;
    }

    struct Set* pSet = getSetAndCheck(pServer, pClient);
    if (!pSet) return;
    if (sismember(pSet, pClient->argv[2]))
        sprintf(pClient->sendBuff, "1");
    else 
        sprintf(pClient->sendBuff, "0");
}

void sinterCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SINTER SET1 SET2 [SET3 ...]");
        return;
    }

    struct Set** pSets = (struct Set**)malloc(sizeof(struct Set*)*(pClient->argc-1));
    struct Set* pRetSet = NULL;

    int flag = 1;
    for (int i = 1; i < pClient->argc; ++i)
    {
        struct ValueObject* pObj;
        pObj = (struct ValueObject*)getValue(
            pServer->pDB->phash[pClient->db_index], pClient->argv[i]);
        if (!pObj || pObj->type != VALUE_TYPE_SET)
        {
            sprintf(pClient->sendBuff, "(error) %s is not a set.", pClient->argv[i]);
            flag = 0;
            break;
        }
        pSets[i-1] = pObj->value.pSet;
    }
    if (flag)
    {
        pRetSet = interSets(pSets, pClient->argc-1);
        formatSet(pRetSet, pClient->sendBuff);
    }
    free(pSets);
    if (pRetSet) 
        free(pRetSet);
}

void sunionCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SUNION SET1 SET2 [SET3 ...]");
        return;
    }

    struct Set** pSets = (struct Set**)malloc(sizeof(struct Set*)*(pClient->argc-1));
    struct Set* pRetSet = NULL;

    int flag = 1;
    for (int i = 1; i < pClient->argc; ++i)
    {
        struct ValueObject* pObj;
        pObj = (struct ValueObject*)getValue(
            pServer->pDB->phash[pClient->db_index], pClient->argv[i]);
        if (!pObj || pObj->type != VALUE_TYPE_SET)
        {
            sprintf(pClient->sendBuff, "(error) %s is not a set.", pClient->argv[i]);
            flag = 0;
            break;
        }
        pSets[i-1] = pObj->value.pSet;
    }
    if (flag)
    {
        pRetSet = unionSets(pSets, pClient->argc-1);
        formatSet(pRetSet, pClient->sendBuff);
    }
    free(pSets);
    if (pRetSet) 
        free(pRetSet);
}

void sremoveCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SREMOVE KEY V1 [V2 ...]");
        return;
    }

    if (existKey(pServer->pDB->phash[pClient->db_index], pClient->argv[1]))  // 该键存在
    {
        struct ValueObject* pObj;
        pObj = (struct ValueObject*)getValue(pServer->pDB->phash[pClient->db_index], 
            pClient->argv[1]);
        int nSuccessed = 0;
        for (int i = 2; i < pClient->argc; ++i)
            nSuccessed += removeSet(pObj->value.pSet, pClient->argv[i]);

        pServer->changed++;
        sprintf(pClient->sendBuff, "[OK] %d", nSuccessed);
    }
}