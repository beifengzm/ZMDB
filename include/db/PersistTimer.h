#pragma once

struct DBServer;
struct PersistTimerArg
{
    struct DBServer* pServer;
    unsigned int n, m;  // n次内修改了m次
};

struct Timer* newPersistTimer(struct DBServer* pServer, int n, int m);
void addPersistTimer(struct DBServer* pServer);