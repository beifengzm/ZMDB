#pragma once

struct Hash;
struct Database;
struct DBServer;
struct DBClient;

struct CommandProc
{
    struct Hash* pMap;
    void (*execteCommand)(struct CommandProc*, struct DBServer*, struct DBClient*);
};

struct CommandProc* getCommandProc();
void freeCommandProc(struct CommandProc*);
