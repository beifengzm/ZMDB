#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include "Server.h"
#include "Debug.h"
#include "Warp.h"
#include "Select.h"

#define BUFSIZE 1024

static void select_loop(struct DBServer* pServer)
{
    zmprintf("select_loop running, server fd: %d, port = %d\n", 
        pServer->listenfd, pServer->port);

    int ret, nAlready;
    int cfd, maxfd;
    char buff[BUFSIZE];
    fd_set curSet, allSet;
    socklen_t client_len;
    struct sockaddr_in client_addr;
    
    FD_ZERO(&allSet);
    FD_SET(pServer->listenfd, &allSet);

    client_len = sizeof(client_addr);
    maxfd = pServer->listenfd;

    zmprintf("Accepting connections......\n");
    for (;;)
    {
        curSet = allSet;
        nAlready = select(maxfd+1, &curSet, NULL, NULL, NULL);
        if (nAlready < 0)    // 发生错误
        {
            perror("select error");
            exit(1);
        }
        else if (nAlready == 0)    // 时间用尽，继续轮询
            continue;
        
        if (FD_ISSET(pServer->listenfd, &curSet))   // 有新的客户端请求连接
        {
            cfd = Accept(pServer->listenfd, (struct sockaddr*)&client_addr, &client_len);
            zmprintf("client %d connect.\n", cfd);
            if (cfd > maxfd) 
                maxfd = cfd;
            FD_SET(cfd, &allSet);

            if (--nAlready == 0)
                continue;
        }

        // 有客户端请求通信
        for (int fd = pServer->listenfd+1; fd <= maxfd; ++fd)
        {
            if (!FD_ISSET(fd, &curSet)) 
                continue;

            ret = read(fd, buff, sizeof(buff));
            if (ret == 0)   // 客户端关闭
            {
                close(fd);
                FD_CLR(fd, &allSet);
                zmprintf("client %d close.\n", fd);
                continue;
            }
            zmprintf("client %d: %s", fd, buff);
            for (int i = 0; i < ret; ++i)
                buff[i] = toupper(buff[i]);
            write(fd, buff, ret);
        }
    }
    
    closeServer(pServer);
}

Select_loop getSelectLoop()
{
    Select_loop loop = select_loop;
    return loop;
}