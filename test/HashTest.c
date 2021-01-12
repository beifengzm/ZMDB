#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"

int main()
{
    struct Hash* phash = newHash();
    char op[31], key[31], value[31];
    int i = 1;
    for (;;)
    {
        scanf("%s", op);
        if (!strcmp(op, "add"))
        {
            sprintf(key, "key%d", i);
            sprintf(value, "value%d", i);
            putKV(phash, key, value, strlen(value)+1);
            i++;
        }
        else if (!strcmp(op, "get"))
        {
            scanf("%s", key);
            char* result = (char*)getValue(phash, key);
            printf("%s\n", result==NULL?"-1":result);
        }
        if (!strcmp(op, "all"))
        {
            void *pkey, *pvalue;
            char* key1, *value1;
            while (iterateHash(phash, &pkey, &pvalue))
            {
                key1 = pkey, value1 = pvalue;
                printf("key: %s, value: %s\n", key1, value1);
            }
        }
        else if (!strcmp(op, "quit"))
            break;
        printf("size: %d, capcity: %d\n", phash->size, phash->capcity);
    }

    freeHash(phash);
    return 0;
}