#pragma once

#define VALUE_TYPE_STR 0
#define VALUE_TYPE_HASH 1
#define VALUE_TYPE_SET 2
#define VALUE_TYPE_LIST 3

struct DBClient;
struct Hash;
struct Set;
struct Timer;

struct ValueObject
{
    int type;
    struct Timer* pTimer;
    union 
    {
        char* str;
        struct Hash* pHash;
        struct Set* pSet;
    }value;   // 注意：此处内存为动态申请，需要手动释放
};

void initValueObject(struct ValueObject* pObject, 
        struct DBClient* pClient, int type);
