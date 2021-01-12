#pragma once

struct DBServer;
typedef void (*Epoll_loop)(struct DBServer*);
Epoll_loop getEpollLoop();