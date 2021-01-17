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

    struct ValueObject* pObject = getObjectAndCreate(pServer, pClient, VALUE_TYPE_STR);
    strcpy(pObject->value.str, pClient->argv[2]);

    sprintf(pClient->sendBuff, "[OK] 1");

    pServer->changed++;
}