#include "PersistTimer.h"
#include "Persist.h"
#include "Server.h"
#include "Timer.h"
#include "Debug.h"
#include "Config.h"

static void persistTimeProc(void* arg)
{
    struct PersistTimerArg* pData = (struct PersistTimerArg*)arg;

    if (pData->pServer->changed >= pData->m)
    {
        zmprintf("Persisting... n = %d, m = %d, changed = %d\n", 
            pData->n, pData->m, pData->pServer->changed);
        persist(pData->pServer);  // n秒内数据库改变了m次，可以进行持久化操作
        pData->pServer->changed = 0;
    }

    // 创建新的持久化定时器
    struct Timer* newTimer = newPersistTimer(pData->pServer, pData->n, pData->m);
    addTimer(pData->pServer->pTimerQueue, newTimer);
}

static void freePersistTimer(struct Timer* pTimer)
{
    struct PersistTimerArg* pData = (struct PersistTimerArg*)pTimer->data;
    free(pData);
    free(pTimer);
}

struct Timer* newPersistTimer(struct DBServer* pServer, int n, int m)
{
    struct Timer* pTimer = (struct Timer*)malloc(sizeof(struct Timer));
    pTimer->interval = n;
    pTimer->timeProc = persistTimeProc;
    pTimer->when = getNowTime()+n*1000;
    pTimer->freeSelf = freePersistTimer;

    struct PersistTimerArg* pData;
    pData = (struct PersistTimerArg*)malloc(sizeof(struct PersistTimerArg));
    pData->pServer = pServer;
    pData->n = n;
    pData->m = m;

    pTimer->data = pData;

    return pTimer;
}

void addPersistTimer(struct DBServer* pServer)
{
    int i = 1;
    int n, m;
    char *pn, *pm;
    char keyn[15], keym[15];
    struct Timer* pTimer;
    for (;;)
    {
        sprintf(keyn, "n%d", i);
        sprintf(keym, "m%d", i);
        pn = GET_CONFIG(pServer->pConfig, keyn);
        pm = GET_CONFIG(pServer->pConfig, keym);
        if (pn==NULL || pm==NULL) break;
        n = atoi(pn), m = atoi(pm);
        pTimer = newPersistTimer(pServer, n, m);
        addTimer(pServer->pTimerQueue, pTimer);
        i++;
    }
    zmprintf("Add %d persistence timers successfully.\n", i-1);
}