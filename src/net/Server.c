#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Database.h"
#include "Timer.h"
#include "Persist.h"
#include "CommandProc.h"
#include "PersistTimer.h"
#include "Server.h"
#include "Select.h"
#include "Epoll.h"
#include "Config.h"
#include "Warp.h"
#include "Debug.h"

struct DBServer* newServer(const char* filename)
{
    zmprintf("Start creating server...\n");
    struct DBServer* pServer;
    pServer = (struct DBServer*)malloc(sizeof(struct DBServer));

    pServer->pConfig = newConfig(filename);

    pServer->port = GET_CONFIG_USHORT(pServer->pConfig,"server_port");
    pServer->serv_addr.sin_family = AF_INET;
    pServer->serv_addr.sin_port = htons(pServer->port);
    pServer->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    pServer->timeNoResuest = GET_CONFIG_INT(pServer->pConfig, "timeNoResuest");
    pServer->epoll_timeout = GET_CONFIG_INT(pServer->pConfig, "epoll_timeout");
    
    pServer->pProc = getCommandProc();
    pServer->pTimerQueue = newTimerQueue();

    
    loadDB(pServer);
    pServer->changed = 0;
    addPersistTimer(pServer);

    // IO type
    if (!strcmp(GET_CONFIG(pServer->pConfig,"iotype"), "select"))
    {
        pServer->iotype = SERVER_IO_SELECT;
        pServer->loop = getSelectLoop();
    }
    else 
    {
        pServer->iotype = SERVER_IO_EPOLL;
        pServer->loop = getEpollLoop();
    }

    pServer->listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(pServer->listenfd, (struct sockaddr*)&pServer->serv_addr, 
            sizeof(pServer->serv_addr));
    
    Listen(pServer->listenfd, 128);
    zmprintf("Creating server done. listenfd: %d, iotype: %s, port: %d\n",
        pServer->listenfd, pServer->iotype==SERVER_IO_EPOLL?"epoll":"select", pServer->port);

    return pServer;
}

void closeServer(struct DBServer* pServer)
{
    close(pServer->listenfd);
    closeDB(pServer->pDB);
    freeCommandProc(pServer->pProc);
    freeConfig(pServer->pConfig);

    free(pServer);
    zmprintf("Closing server done.\n");
}