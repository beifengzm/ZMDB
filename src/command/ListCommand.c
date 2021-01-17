#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ListCommand.h"
#include "ValueObject.h"
#include "LinkedList.h"
#include "Client.h"
#include "Server.h"

void lpushCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: LPUSH KEY V1 [V2 ...]");
        return;
    }

    struct ValueObject* pObject = getObjectAndCreate(pServer, pClient, VALUE_TYPE_LIST);
    for (int i = 2; i < pClient->argc; i++)
        lpush(pObject->value.plist, pClient->argv[i], strlen(pClient->argv[i])+1);

    pServer->changed++;
    sprintf(pClient->sendBuff, "[OK] %d", pClient->argc-2);
}

void rpushCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc < 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: RPUSH KEY V1 [V2 ...]");
        return;
    }

    struct ValueObject* pObject = getObjectAndCreate(pServer, pClient, VALUE_TYPE_LIST);
    for (int i = 2; i < pClient->argc; i++)
        rpush(pObject->value.plist, pClient->argv[i], strlen(pClient->argv[i])+1);

    pServer->changed++;
    sprintf(pClient->sendBuff, "[OK] %d", pClient->argc-2);
}

void lpopCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: LPOP KEY");
        return;
    }

    struct LinkedList* plist = (struct LinkedList*)getTypeObjectAndCheck(pServer, pClient, VALUE_TYPE_LIST);
    if (!plist)
        return;

    pServer->changed++;
    if (plist->size > 0)
        sprintf(pClient->sendBuff, "1) %s", (char*)front(plist));
    else
        sprintf(pClient->sendBuff, "null");
    lpop(plist);
}

void rpopCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: RPOP KEY");
        return;
    }

    struct LinkedList* plist = (struct LinkedList*)getTypeObjectAndCheck(pServer, pClient, VALUE_TYPE_LIST);
    if (!plist)
        return;

    pServer->changed++;
    if (plist->size > 0)
        sprintf(pClient->sendBuff, "1) %s", (char*)back(plist));
    else
        sprintf(pClient->sendBuff, "null");
    rpop(plist);
}

void lindexCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 3)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: LINDEX KEY INDEX");
        return;
    }

    struct LinkedList* plist = (struct LinkedList*)getTypeObjectAndCheck(pServer, pClient, VALUE_TYPE_LIST);
    if (!plist)
        return;

    int index = atoi(pClient->argv[2]);
    printf("index = %d, list size: %d\n", index, plist->size);
    if (index > (int)plist->size)
        sprintf(pClient->sendBuff, "(error) Out of list index range.");
    else
        sprintf(pClient->sendBuff, "[value] %s", 
            (char*)getValueByIndex(plist, index));
}

void lsizeCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 2)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: LSIZE KEY");
        return;
    }

    struct LinkedList* plist = (struct LinkedList*)getTypeObjectAndCheck(pServer, pClient, VALUE_TYPE_LIST);
    if (!plist)
        return;

    sprintf(pClient->sendBuff, "(size) %d", getListSize(plist));
}

void lrangeCommand(struct DBServer* pServer, struct DBClient* pClient)
{
    if (pClient->argc != 4)
    {
        sprintf(pClient->sendBuff, "Argc error! usege: LRANGE KEY START END");
        return;
    }

    struct LinkedList* plist = (struct LinkedList*)getTypeObjectAndCheck(pServer, pClient, VALUE_TYPE_LIST);
    if (!plist)
        return;

    int start = atoi(pClient->argv[2]), end = atoi(pClient->argv[3]);
    if ((end != -1 && start > end) || start < 0 || end > (int)plist->size)
    {
        sprintf(pClient->sendBuff, "(error) Index error.");
        return;
    }

    char *result;
    sprintf(pClient->sendBuff, "(list)\n");
    int pos = strlen(pClient->sendBuff);
    int i = start;
    char buff[512];
    while (rangeList(plist, start, end, (void**)&result))
    {
        sprintf(buff, "%d) %s\n", i, result);
        sprintf(pClient->sendBuff+pos, "%s", buff);
        pos += strlen(buff);
        i++;
    }
    pClient->sendBuff[pos-1] = '\0';
}