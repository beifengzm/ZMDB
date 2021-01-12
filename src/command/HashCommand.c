#include <stdio.h>

#include "HashCommand.h"
#include "CCommand.h"
#include "Server.h"
#include "Client.h"
#include "Hash.h"
#include "Database.h"
#include "Timer.h"
#include "ValueObject.h"

void hsetCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 4 || (pClient->argc-2)&1) 
    {
        sprintf(pClient->sendBuff, "Argc error! usege: HSET KEY K1 V1 [K2] [V2] ... [KN] [VN]");
        return;
    }

    // 如果该键存在，则删除
    removeOldMapIfExist(pServer, pClient);

    struct ValueObject obj;
    initValueObject(&obj, pClient, VALUE_TYPE_HASH);

    putKV(pServer->pDB->phash[pClient->db_index], pClient->argv[1], &obj, sizeof(struct ValueObject));
    sprintf(pClient->sendBuff, "[OK] 1");

    pServer->changed++;
}