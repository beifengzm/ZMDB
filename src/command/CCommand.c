#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "CCommand.h"
#include "Server.h"
#include "Client.h"
#include "Database.h"
#include "Set.h"
#include "Hash.h"
#include "Timer.h"
#include "Debug.h"
#include "ValueObject.h"
#include "LinkedList.h"
#include "ObjectTimer.h"

void delCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2) 
    {
        sprintf(pClient->sendBuff, "Argc error! usege: REMOVE KEY");
        return;
    }

    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);
    if (!pObj)
        return;

    // 该键存在，则删除
    if (pObj->pTimer)
        removeTimer(pServer->pTimerQueue, pObj->pTimer);
    removeMapFromDB(pServer->pDB, pClient->argv[1], pClient->db_index);

    sprintf(pClient->sendBuff, "[OK] 1");
    pServer->changed++;
}

void keysCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    int i = 1;
    char* key;
    struct ValueObject* pObj;
    if (pServer->pDB->phash[pClient->db_index]->size == 0)
    {
        sprintf(pClient->sendBuff, "No keys");
        return;
    }

    char* pStr = pClient->sendBuff;
    char buff[512], typeStr[25];
    while (iterateHash(pServer->pDB->phash[pClient->db_index], (void**)&key, (void**)&pObj))
    {
        switch (pObj->type)
        {
            case VALUE_TYPE_STR:
                strcpy(typeStr, "str");
                break;

            case VALUE_TYPE_HASH:
                strcpy(typeStr, "hash");
                break;

            case VALUE_TYPE_SET:
                strcpy(typeStr, "set");
                break;

            case VALUE_TYPE_LIST:
                strcpy(typeStr, "list");
                break;
            
            default:
                strcpy(typeStr, "unknown");
                break;
        }
        char* ch = i==pServer->pDB->phash[pClient->db_index]->size?"":"\n";
        sprintf(buff, "%d)\"%s\"(%s)%s", i, key, typeStr, ch);
        sprintf(pStr, "%s", buff);
        pStr += strlen(buff);
        i++;
    }
}

void getCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: GET KEY");
        return;
    }

    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);
    if (!pObj) 
        return;

    switch (pObj->type)
    {
        case VALUE_TYPE_STR:
            sprintf(pClient->sendBuff, "(string) %s", pObj->value.str);
            break;
    
        case VALUE_TYPE_HASH:
        {
            char *tmpKey, *tmpValue;
            sprintf(pClient->sendBuff, "(hash)");
            int pos = strlen(pClient->sendBuff);
            while (iterateHash(pObj->value.pHash, (void**)&tmpKey, (void**)&tmpValue))
            {
                sprintf(pClient->sendBuff+pos, " %s:%s", tmpKey, tmpValue);
                pos += strlen(tmpKey)+strlen(tmpValue)+2;
            }
            break;
        }

        case VALUE_TYPE_SET:
        {
            char *tmpKey;
            sprintf(pClient->sendBuff, "(set)\n");
            int pos = strlen(pClient->sendBuff);
            int i = 1;
            char buff[512];
            while (iterateSet(pObj->value.pSet, (void**)&tmpKey))
            {
                sprintf(buff, "%d) %s\n", i, tmpKey);
                sprintf(pClient->sendBuff+pos, "%s", buff);
                pos += strlen(buff);
                i++;
            }
            pClient->sendBuff[pos-1] = '\0';
            break;
        }

        case VALUE_TYPE_LIST:
        {
            char *result;
            sprintf(pClient->sendBuff, "(list)\n");
            int pos = strlen(pClient->sendBuff);
            int i = 1;
            char buff[512];
            while (iterateList(pObj->value.plist, (void**)&result))
            {
                sprintf(buff, "%d) %s\n", i, result);
                sprintf(pClient->sendBuff+pos, "%s", buff);
                pos += strlen(buff);
                i++;
            }
            pClient->sendBuff[pos-1] = '\0';
            break;
        }

        default: 
            break;
    }
}

void enterCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: ENTER INDEX");
        return;
    }

    char* pStr;
    int flag = 1, index = 0;
    for (pStr = pClient->argv[1]; *pStr; ++pStr)
    {
        if (*pStr<'0' || *pStr>'9')
        {
            flag = 0;
            break;
        }
        index = index*10+(*pStr-'0');
    }
    if (index >= pServer->pDB->nDB) flag = 0;

    if (!flag)
    {
        sprintf(pClient->sendBuff, "%s", "DB index error!");
    }
    else
    {
        pClient->db_index = index;
        sprintf(pClient->sendBuff, "Enter db(%d) successfully", index);
    }
}

// 移除过期腱
void persistCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: PERSIST KEY");
        return;
    }

    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);
    if (!pObj) 
        return;

    if (pObj->pTimer)
    {   
        removeTimer(pServer->pTimerQueue, pObj->pTimer);
        pObj->pTimer = NULL;
    }
        
    sprintf(pClient->sendBuff, "(ttl) %s: -1(ms)", pClient->argv[1]);

    pServer->changed++;
}

// 给对应的Object设定定时器(millionseconds)
void mexpireCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: EXPIRE KEY MILLIONSECONDS");
        return;
    }

    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);
    if (!pObj) 
        return;

    char* p = pClient->argv[2];
    while (*p)
    {
        if (!isdigit(*p))
        {
            sprintf(pClient->sendBuff, "Arg[2] should be a digit! usege: MEXPIRE KEY MILLIONSECONDS");
            return;
        }
        p++;
    }

    // 移除旧的过期腱
    if (pObj->pTimer)
    {
        removeTimer(pServer->pTimerQueue, pObj->pTimer);
        pObj->pTimer = NULL;
    }
        

    // 添加新的过期腱
    long when = getExpireTime(atol(pClient->argv[2]));

    struct Timer* pTimer = newObjectTimer(pServer, when, pClient->argv[1], pClient->db_index);
    
    addTimer(pServer->pTimerQueue, pTimer);
    pObj->pTimer = pTimer;

    long leftTime = pObj->pTimer->when - getNowTime();
    sprintf(pClient->sendBuff, "(ttl) %s: %ld(ms)", pClient->argv[0], leftTime);

    pServer->changed++;
}

// 给对应的Object设定定时器(seconds)
void expireCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: EXPIRE KEY SECONDS");
        return;
    }

    char* p = pClient->argv[2];
    while (*p)
    {
        if (!isdigit(*p))
        {
            sprintf(pClient->sendBuff, "Arg[2] should be a digit! usege: EXPIRE KEY SECONDS");
            return;
        }
        p++;
    }

    char* newargv = (char*)malloc(strlen(pClient->argv[2])+1+3);
    char* p1 = newargv, *p2 = pClient->argv[2];
    for (; *p2; ++p1, ++p2)
        *p1 = *p2;
    for (int i = 0; i < 3; ++i) *p1++ = '0';
    *p1 = '\0';
    free(pClient->argv[2]);
    pClient->argv[2] = newargv;

    mexpireCommand(pServer, pClient);
}

// 查看对应键还有多久到期
void ttlCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: TTL KEY");
        return;
    }

    struct ValueObject* pObj = getObjectAndCheck(pServer, pClient);
    if (!pObj) 
        return;

   if (pObj->pTimer == NULL)
        sprintf(pClient->sendBuff, "-1");
    else 
    {
        long left = pObj->pTimer->when-getNowTime();
        sprintf(pClient->sendBuff, "%ld(ms)", left);
    }
}

// 查看当前数据库中所有键还有多久到期
void ttlsCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 1)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: TTLS");
        return;
    }

    if (pServer->pDB->phash[pClient->db_index]->size == 0)
    {
        sprintf(pClient->sendBuff, "No keys");
        return;
    }

    int i = 1;
    char buff[512];
    char* key, *pStr = pClient->sendBuff;
    struct ValueObject* pObj;

    long now = getNowTime();
    while (iterateHash(pServer->pDB->phash[pClient->db_index], (void**)&key, (void**)&pObj))
    {
        long leftTime = -1;
        if (pObj->pTimer) 
            leftTime = pObj->pTimer->when - now;

        char* ch = i==pServer->pDB->phash[pClient->db_index]->size?"":"\n";
        sprintf(buff, "%d)\"%s\": %ld%s", i, key, leftTime, ch);
        sprintf(pStr, "%s", buff);
        pStr += strlen(buff);
        i++;
    }
}