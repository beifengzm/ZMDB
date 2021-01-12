#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Server.h"
#include "Database.h"
#include "ValueObject.h"
#include "Persist.h"
#include "Config.h"
#include "Hash.h"

int main()
{
    struct DBServer* pServer = newServer("../zmdb.conf");
    
    FILE* fp;
    if ((fp = fopen(GET_CONFIG(pServer->pConfig, "dbfilename"), "wb")) == NULL)
    {
        perror("File open filed!");
        exit(0);
    }

    saveHash(fp, pServer->pDB->phash[0]);
    fclose(fp);

    fp = fopen(GET_CONFIG(pServer->pConfig, "dbfilename"), "rb");
    struct Hash* phash = loadHash(fp, pServer, 0);
    printf("hash size: %u\n", phash->size);
    fclose(fp);

    char op[31], key[31];
    struct ValueObject* pObj;
    for (;;)
    {
        scanf("%s", op);
        if (!strcmp(op, "get"))
        {
            scanf("%s", key);
            pObj = (struct ValueObject*)getValue(phash, key);
            printf("%s\n", pObj==NULL?"-1":pObj->value.str);
        }
        if (!strcmp(op, "all"))
        {
            char* key1;
            while (iterateHash(phash, (void**)&key1, (void**)&pObj))
            {
                printf("key: %s, value: %s\n", key1, pObj->value.str);
            }
        }
        else if (!strcmp(op, "quit"))
            break;
        printf("size: %d, capcity: %d\n", phash->size, phash->capcity);
    }

    return 0;
}