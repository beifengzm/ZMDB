#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "Epoll.h"
#include "Server.h"
#include "Persist.h"
#include "CommandProc.h"
#include "Priority_queue.h"
#include "Config.h"
#include "Timer.h"
#include "Debug.h"
#include "Warp.h"
#include "Client.h"

#define MAX_EVENTS 1024
#define MIN(a,b) (a)<(b)?(a):(b)

struct NodeInfo
{
    int epollfd;
    void (*callback)(struct DBServer*, void*);
    time_t last_active;  // 上次被监听到的时间
    struct DBClient* pClient;

    struct NodeInfo* last;
    struct NodeInfo* next;
};

static int nNode = 0;
static struct NodeInfo *head, *pEnd, *pIter;

static struct NodeInfo* create_node(int fd, int epollfd, void (*callback)(struct DBServer*, void*));
static void remove_node(struct NodeInfo* pNode);
// static int isEmpty();
static void senddata(struct DBServer*, void*);
static void recvdata(struct DBServer*, void*);
static void listen_handler(struct DBServer*, void*);
static void epoll_loop(struct DBServer*);

Epoll_loop getEpollLoop()
{
    Epoll_loop loop = epoll_loop;
    return loop;
}

static void epoll_loop(struct DBServer* pServer)
{
    zmprintf("epoll_loop running, server fd: %d, port = %d\n", 
        pServer->listenfd, pServer->port);
    
    int epollfd, nfd;
    int checkpos = 0;

    struct epoll_event events[MAX_EVENTS];
    epollfd = epoll_create(MAX_EVENTS);    // 创建红黑树的根节点

    struct NodeInfo* pListenNode = create_node(pServer->listenfd, epollfd, listen_handler);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = pServer->listenfd;
    event.data.ptr = pListenNode;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, pServer->listenfd, &event);   // 向树中添加监听节点

    zmprintf("Accepting connections......\n");
    for (;;)
    {
        // // 关闭长时间未被监听到的客户端
        time_t now = time(NULL); 
        while (pIter && ++checkpos < 100)
        {
            int duration = now - pIter->last_active;
            int bRemove = 0;
            if (pServer->timeNoResuest != -1 && duration >= pServer->timeNoResuest)
            {
                zmprintf("Client %d was closed because it was not monitored for a long time.\n", pIter->pClient->fd);
                epoll_ctl(epollfd, EPOLL_CTL_DEL, pIter->pClient->fd, NULL);
                bRemove = 1;
                remove_node(pIter);
                pIter = NULL;
            }
            if (!bRemove)
                pIter = pIter->next;
        }
        if (pIter == NULL)
            pIter = head->next;
        if (checkpos == 100)
            checkpos = 0;

        long leftTime = nearestLeftTime(pServer->pTimerQueue);  // leftTime<=0表示已有时间事件发生
        long waitTime = leftTime==-1?pServer->epoll_timeout:MIN(leftTime, pServer->epoll_timeout);
        nfd = epoll_wait(epollfd, events, MAX_EVENTS, waitTime);  // -1表示阻塞等待,0表示立即返回
        if (nfd < 0)    // 发生错误
        {
            perror("epoll error");
            exit(1);
        }

        for (int i = 0; i < nfd; ++i)
        {
            struct NodeInfo* pInfo = (struct NodeInfo*)events[i].data.ptr;
            pInfo->callback(pServer, pInfo);
        } 

        runTimerQueue(pServer->pTimerQueue, GET_CONFIG_INT(pServer->pConfig,"maxnumOfRemoved"));
        
        // if (nfd == 0 && isEmpty())
        // {
        //     zmprintf("Server closing because it was not monitored for a long time.\n");
        //     break;
        // }
    }
    
    close(epollfd);
    remove_node(pListenNode);
    closeServer(pServer);
}

static struct NodeInfo* create_node(int fd, int epollfd, void (*callback)(struct DBServer*, void*))
{
    struct NodeInfo* pNode;

    pNode = (struct NodeInfo*)malloc(sizeof(struct NodeInfo));
    pNode->pClient = newClient(fd);
    pNode->epollfd = epollfd;
    pNode->callback = callback;
    pNode->last_active = time(NULL);
    pNode->next = NULL;

    // 设置为ET模式，将fd设置为非阻塞的(non-blocking)
    int flags = fcntl(fd, F_GETFL,0);
    fcntl(fd, F_SETFL, flags|O_NONBLOCK);

    if (head == NULL)
    {
        pEnd = head = pNode;
        pNode->last = NULL;
    }
    else
    {
        pEnd->next = pNode;
        pNode->last = pEnd;
        pEnd = pNode;

        nNode++;
    }

    return pNode;
}

static void remove_node(struct NodeInfo* pNode)
{
    if (pNode == head)
    {
        closeClient(pNode->pClient);
        free(pNode);
        return;
    }
    if (pIter == pNode)
        pIter = pIter->next;
    if (pNode == pEnd)
        pEnd = pNode->last;
    else
        pNode->next->last = pNode->last;
    
    pNode->last->next = pNode->next;
    closeClient(pNode->pClient);
    free(pNode);
    nNode--;
}

// static int isEmpty()
// {
//     return nNode==0?1:0;
// }

static void senddata(struct DBServer* pServer, void* args)
{
    struct NodeInfo* pInfo = (struct NodeInfo*)args;
    struct DBClient* pClient = pInfo->pClient;
    int connfd = pClient->fd;

    // 发送数据
    pServer->pProc->execteCommand(pServer->pProc,pServer, pClient);
    send(connfd, pClient->sendBuff, sizeof(pClient->sendBuff), 0);

    epoll_ctl(pInfo->epollfd, EPOLL_CTL_DEL, connfd, NULL);

    pInfo->callback = recvdata;

    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = connfd;
    event.data.ptr = pInfo;

    epoll_ctl(pInfo->epollfd, EPOLL_CTL_ADD, connfd, &event);
}

static void recvdata(struct DBServer* pServer, void* args)
{
    int ret, connfd;
    struct NodeInfo* pInfo = (struct NodeInfo*)args;
    struct DBClient* pClient = pInfo->pClient;
    connfd = pClient->fd;

    for (;;)
    {
        ret = recv(connfd, pClient->recvBuff, sizeof(pClient->recvBuff), 0);
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
    }
    if (ret == 0)   // 客户端关闭
    {
        epoll_ctl(pInfo->epollfd, EPOLL_CTL_DEL, connfd, NULL);
        remove_node(pInfo);
        return;
    }
    zmprintf("client %d: %s\n", connfd, pClient->recvBuff);
    pClient->parseCommond(pClient);
    
    epoll_ctl(pInfo->epollfd, EPOLL_CTL_DEL, connfd, NULL);

    pInfo->callback = senddata;
    pInfo->last_active = time(NULL);   // 更新活跃时间

    struct epoll_event event;
    event.events = EPOLLOUT|EPOLLET;
    event.data.fd = connfd;
    event.data.ptr = pInfo;

    epoll_ctl(pInfo->epollfd, EPOLL_CTL_ADD, connfd, &event);
}

static void listen_handler(struct DBServer* pServer, void* args)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    struct NodeInfo* pInfo = (struct NodeInfo*)args;
    int listenfd = pInfo->pClient->fd;

    int connfd = Accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
    zmprintf("client(fd=%d) connect successfully.\n", connfd);

    struct NodeInfo* pConnNode = create_node(connfd, pInfo->epollfd, recvdata);
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = listenfd;
    event.data.ptr = pConnNode;
    epoll_ctl(pInfo->epollfd, EPOLL_CTL_ADD, connfd, &event);
}