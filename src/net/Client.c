#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "Client.h"
#include "Debug.h"
#include "Warp.h"

static void parseCommond(struct DBClient* pClient)
{
    for (unsigned int i = 0; i < pClient->argc; ++i)
        free(pClient->argv[i]);
    if (pClient->argv) 
        free(pClient->argv);
    pClient->argc = 0;
    pClient->argv = NULL;

    if (!*pClient->recvBuff) return;  // 空串

    const char* pStr;
    for (pStr = pClient->recvBuff; *pStr;)
    {
        while (*pStr && *pStr==' ') pStr++;
        if (*pStr) pClient->argc++;
        while (*pStr && *pStr!=' ') pStr++;
    }
    pClient->argv = (char**)malloc(sizeof(char*)*pClient->argc);

    char buff[256];
    int p, count = 0;
    for (pStr = pClient->recvBuff;;)
    {
        while (*pStr && *pStr==' ') pStr++;
        for (p = 0; *pStr && *pStr!=' '; ++p,++pStr)
        {
            char ch = *pStr;
            if (count == 0 && ch >= 'A' && ch <= 'Z')
                ch = tolower(ch);
            buff[p] = ch;
        }
        if (p == 0) break;
        buff[p] = '\0';
        pClient->argv[count] = (char*)malloc(p+1);
        strcpy(pClient->argv[count], buff);
        count++;
    }
}

struct DBClient* newClient(int fd)
{
    struct DBClient* pClient;
    pClient = (struct DBClient*)malloc(sizeof(struct DBClient));

    bzero(pClient->sendBuff, sizeof(pClient->sendBuff));
    bzero(pClient->recvBuff, sizeof(pClient->recvBuff));

    pClient->fd = fd;
    pClient->db_index = 0;

    pClient->argc = 0;
    pClient->argv = NULL;

    pClient->parseCommond = parseCommond;
    zmprintf("Creating client done, fd = %d.\n", pClient->fd);
    
    return pClient;
}

void closeClient(struct DBClient* pClient)
{
    for (unsigned int i = 0; i < pClient->argc; ++i)
    {
        free(pClient->argv[i]);
    }
    if (pClient->argv) 
        free(pClient->argv);
    close(pClient->fd);
    zmprintf("Closing client(fd=%d) done.\n", pClient->fd);
    free(pClient);
}