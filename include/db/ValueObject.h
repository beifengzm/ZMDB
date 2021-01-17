#pragma once

#define VALUE_TYPE_STR 0
#define VALUE_TYPE_HASH 1
#define VALUE_TYPE_SET 2
#define VALUE_TYPE_LIST 3

struct DBClient;
struct Hash;
struct Set;
struct Timer;
struct DBServer;
struct LinkedList;

struct ValueObject
{
    int type;
    struct Timer* pTimer;
    union 
    {
        char* str;
        struct Hash* pHash;
        struct Set* pSet;
        struct LinkedList* plist;
    }value;   // 注意：此处内存为动态申请，需要手动释放
};

// 删除旧的键值对(如果存在)
void removeOldMapIfExist(struct DBServer* pServer, struct DBClient* pClient);
struct ValueObject* getObjectAndCheck(struct DBServer* pServer, struct DBClient* pClient);
void* getTypeObjectAndCheck(struct DBServer* pServer, struct DBClient* pClient, int type);
struct ValueObject* getObjectAndCreate(struct DBServer* pServer,
         struct DBClient* pClient, int type);
