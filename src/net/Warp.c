#include "Warp.h"

int Socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd == -1) 
    {
        perror("Socket error");
        exit(1);
    }
    return fd;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t c)
{
    int ret;
    ret = bind(sockfd, addr, c);
    if (ret == -1) 
    {
        perror("Bind error");
        exit(1);
    }
    return ret;
}

int Listen(int sockfd, int backlog)
{
    int ret;
    ret = listen(sockfd, backlog);
    if (ret == -1) 
    {
        perror("Listen error");
        exit(1);
    }
    return ret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int fd;
    fd = accept(sockfd, addr, addrlen);
    if (fd == -1) 
    {
        perror("Listen error");
        exit(1);
    }
    return fd;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int fd;
    fd = Connect(sockfd, addr, addrlen);
    if (fd == -1) 
    {
        perror("Listen error");
        exit(1);
    }
    return fd;
}
