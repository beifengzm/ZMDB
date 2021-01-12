#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Config.h"
#include "Debug.h"
#include "Hash.h"

#define MAX_LINE 512

struct DBConfig* newConfig(const char* filename)
{
    zmprintf("Start loading config file.\n");
    struct DBConfig* pConfig;
    pConfig = (struct DBConfig*)malloc(sizeof(struct DBConfig));
    pConfig->phash = newHash();

    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        perror("file open fail");
        exit(0);
    }

    char buff[MAX_LINE], key[MAX_LINE], value[MAX_LINE];
    while (fgets(buff, MAX_LINE, fp) != NULL)
    {
        char* str = buff;
        while (*str==' ') str++;
        if (*str=='#' || *str=='\n') continue;
        sscanf(str, "%s%s", key, value);
        int keylen = strlen(key), valuelen = strlen(value);
        key[--keylen]='\0';
        putKV(pConfig->phash, key, value, valuelen+1);
    }
    fclose(fp);
    zmprintf("Loading config file Done.\n");

    return pConfig;
}

void freeConfig(struct DBConfig* pConfig)
{
    freeHash(pConfig->phash);
    free(pConfig);
    zmprintf("Closing config done.\n");
}

char* getConfig(const struct DBConfig* pConfig, const void* key)
{
    return (char*)getValue(pConfig->phash, key);
}