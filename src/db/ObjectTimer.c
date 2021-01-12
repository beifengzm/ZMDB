#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "ObjectTimer.h"
#include "Timer.h"
#include "Server.h"
#include "Client.h"
#include "Hash.h"
#include "Database.h"

static void objectTimeProc(void* arg)
{
    struct ObjectTimerArg* pData = (struct ObjectTimerArg*)arg;
    removeMapFromDB(pData->pDB, pData->key, pData->index);
}

static void freeObjectTimer(struct Timer* pTimer)
{
    struct ObjectTimerArg* pData;
    pData = (struct ObjectTimerArg*)pTimer->data;
    free(pData->key);
    free(pData);
    free(pTimer);
}

struct Timer* newObjectTimer(struct DBServer* pServer, long when,
         const char* key, int index)
{
    struct Timer* pTimer = (struct Timer*)malloc(sizeof(struct Timer));
    pTimer->interval = 0;
    pTimer->timeProc = objectTimeProc;
    pTimer->when = when;
    
    struct ObjectTimerArg* pData;
    pData = (struct ObjectTimerArg*)malloc(sizeof(struct ObjectTimerArg));
    pData->pDB = pServer->pDB;
    pData->index = index;
    pData->key = (char*)malloc(strlen(key)+1);
    strcpy(pData->key, key);

    pTimer->data = pData;
    pTimer->freeSelf = freeObjectTimer;

    return pTimer;
}