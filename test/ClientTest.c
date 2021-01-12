#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "Warp.h"

#define SERV_PORT 9526

int main(int argc, char** argv)
{
    int sfd;

    char buff[1024], str[1024];

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    sfd = Socket(AF_INET, SOCK_STREAM, 0);

    connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    int count = 0;
    time_t start = time(NULL);
    while (1)
    {
        // sprintf(str, "Hello_%d\n", count++);
        fgets(str,sizeof(str),stdin);
        int len = strlen(str);
        len = len>0?len-1:0;
        str[len] = '\0';
        write(sfd, str, strlen(str)+1);
        if (!strcmp(str, "disconnect"))
            break;
        read(sfd, buff, sizeof(buff));
        printf("%s\n", buff);
        // usleep(100000);
        if (count == 100000) break;
    }
    printf("durration = %ld\n.", time(NULL)-start);

    close(sfd);

    return 0;
}