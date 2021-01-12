#include <stdio.h>
#include <string.h>

#include "Debug.h"
#include "Config.h"

int main()
{
    struct DBConfig* pConfig = newConfig("../zmdb.conf");
    char key[101];
    for (;;)
    {
        scanf("%s", key);
        if (!strcmp(key, "quit")) break;
        char* result = GET_CONFIG(pConfig, key);
        printf("%s\n", result==NULL?"No result":result);
    }
    freeConfig(pConfig);

    return 0;
}