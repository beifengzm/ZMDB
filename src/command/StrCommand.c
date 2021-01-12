#include <stdio.h>

#include "StrCommand.h"
#include "CCommand.h"
#include "Server.h"
#include "Client.h"
#include "Hash.h"
#include "Database.h"
#include "ValueObject.h"

void setCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 3) 
    {
        sprintf(pClient->sendBuff, "Argc error! usege: SET KEY VALUE");
        return;
    }

    removeOldMapIfExist(pServer, pClient);

    struct ValueObject obj;
    initValueObject(&obj, pClient, VALUE_TYPE_STR);

    putKV(pServer->pDB->phash[pClient->db_index], pClient->argv[1], &obj, sizeof(struct ValueObject));
    sprintf(pClient->sendBuff, "[OK] 1");

    pServer->changed++;
}