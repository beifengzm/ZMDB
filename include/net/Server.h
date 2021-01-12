#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IO_EPOLL 0
#define SERVER_IO_SELECT 1

struct DBConfig;
struct Database;
struct CommandProc;
struct PriorityQueue;

struct DBServer
{
    int listenfd;
    int timeNoResuest;   // 客户端无请求的最长等待时间，超过此时间将关闭该客户端
    int epoll_timeout;   // 用于epoll_wait函数的__timeout参数，表示最长阻塞时间
    int iotype;
    unsigned short port;  // 端口号
    struct sockaddr_in serv_addr;

    //Database
    struct Database* pDB;

    unsigned int changed;  // 上次持久化以来更改的次数

    struct DBConfig* pConfig;

    struct CommandProc* pProc;
    struct PriorityQueue* pTimerQueue;

    void (*loop)(struct DBServer*);
};

struct DBServer* newServer(const char* filename);
void closeServer(struct DBServer* pServer);