#include <stdlib.h>
#include <string.h>

#include "ValueObject.h"
#include "Client.h"
#include "Hash.h"
#include "Set.h"

void initValueObject(struct ValueObject* pObject, 
        struct DBClient* pClient, int type)
{
    switch (type)
    {
        case VALUE_TYPE_STR:
            pObject->type = VALUE_TYPE_STR;
            pObject->pTimer = NULL;
            pObject->value.str = (char*)malloc(strlen(pClient->argv[2])+1);
            strcpy(pObject->value.str, pClient->argv[2]);
            break;
        
        case VALUE_TYPE_HASH:
            pObject->type = VALUE_TYPE_HASH;
            pObject->pTimer = NULL;
            pObject->value.pHash = newHash();
            for (int i = 2; i < pClient->argc; i+=2)
            {
                putKV(pObject->value.pHash, pClient->argv[i],
                    pClient->argv[i+1], strlen(pClient->argv[i+1])+1);
            }
            break;

        case VALUE_TYPE_SET:
            pObject->type = VALUE_TYPE_SET;
            pObject->pTimer = NULL;
            pObject->value.pSet = newSet();
            for (int i = 2; i < pClient->argc; i++)
            {
                addSet(pObject->value.pSet, pClient->argv[i]);
            }
            break;
        
        default:
            break;
    }
}