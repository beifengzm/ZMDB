#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFSIZE 1024

struct DBClient
{
    int fd;
    int db_index;
    char sendBuff[BUFFSIZE], recvBuff[BUFFSIZE];

    /*** Command parser  ***/
    int argc;
    char** argv;
    void (*parseCommond)(struct DBClient*);
};

struct DBClient* newClient(int fd);
void closeClient(struct DBClient*);
