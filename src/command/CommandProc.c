#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "CommandProc.h"
#include "StrCommand.h"
#include "HashCommand.h"
#include "SetCommand.h"
#include "ListCommand.h"
#include "CCommand.h"
#include "Hash.h"
#include "Server.h"
#include "Client.h"
#include "Debug.h"

typedef void (*CommandProcFunc)(struct DBServer*, struct DBClient*);
struct FuncMapStruct
{
    char* name;
    CommandProcFunc functor;
};

const struct FuncMapStruct functors[] = 
{
   {"set", setCommand}, 
   {"hset", hsetCommand},
   {"lpush", lpushCommand},
   {"rpush", rpushCommand},
   {"lpop", lpopCommand},
   {"rpop", rpopCommand},
   {"lindex", lindexCommand},
   {"lsize", lsizeCommand},
   {"lrange", lrangeCommand},
   {"sadd", saddCommand},
   {"scard", scardCommand},
   {"sdiff", sdiffCommand},
   {"sinter", sinterCommand},
   {"sunion", sunionCommand},
   {"sremove", sremoveCommand},
   {"sismember", sismemberCommand},
   {"del", delCommand},
   {"keys", keysCommand},
   {"get", getCommand},
   {"enter", enterCommand},
   {"mexpire", mexpireCommand},
   {"expire", expireCommand},
   {"persist", persistCommand},
   {"ttl", ttlCommand},
   {"ttls", ttlsCommand}
};

static void execteCommand(struct CommandProc* pProc, 
        struct DBServer* pServer, struct DBClient* pClient)
{
    CommandProcFunc* pFunc; 
    pFunc = (CommandProcFunc*)getValue(pProc->pMap, pClient->argv[0]);
    if (pFunc == NULL)
        sprintf(pClient->sendBuff, "Command error: No such command");
    else 
        (*pFunc)(pServer, pClient);
}

struct CommandProc* getCommandProc()
{
    struct CommandProc* pProc;
    pProc = (struct CommandProc*)malloc(sizeof(struct CommandProc));

    pProc->pMap = newHash();
    pProc->execteCommand = execteCommand;

    int nProc = sizeof(functors)/sizeof(struct FuncMapStruct);
    for (int i = 0; i < nProc; ++i)
    {
        putKV(pProc->pMap,functors[i].name, &functors[i].functor, sizeof(CommandProcFunc));
    }

    return pProc;
}

void freeCommandProc(struct CommandProc* pProc)
{
    freeHash(pProc->pMap);
    free(pProc);
    zmprintf("Freeing CommandProc done.\n");
}
