#pragma once

struct Timer;
struct Database;
struct DBServer;
struct DBClient;

struct ObjectTimerArg
{
    struct Database* pDB;
    int index;
    char* key;
};

// 服务器, 到期时间，定时器对应的键，定时器对应的数据库索引
struct Timer* newObjectTimer(struct DBServer* pServer, long when,
         const char* key, int index);
