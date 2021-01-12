#pragma once

typedef unsigned int nbyte_t;

struct HashEntry
{
    void* key;
    void* value;
    nbyte_t nKey;      // key在内存中所占的字节数
    nbyte_t nValue;      // value在内存中所占的字节数
    struct HashEntry* next;  // 冲突链表
    struct HashEntry* listlast, *listnext;  // 用于遍历
};

struct Hash
{
    struct HashEntry** hashTable;
    unsigned int capcity;    // 桶容量
    unsigned int size;       // 当前entry个数   

    struct HashEntry* pHead, *pRear;  // 链表头和尾
    struct HashEntry* iter;
    int iterStart;
};

struct Hash* newHash();
int putKV(struct Hash* phash, const void* key, const void* value, nbyte_t nValue);
int setValue(struct Hash* phash, const void* key, const void* value, nbyte_t nValue);
int sizeHash(struct Hash* phash);
int capcityHash(struct Hash* phash);
void* getValue(struct Hash* phash, const void* key);
int existKey(struct Hash* phash, const void* key);
unsigned int getSize(struct Hash* phash);
int removeKey(struct Hash* phash, const void* key);
void freeHash(struct Hash* phash);
int iterateHash(struct Hash* phash, void** key, void** value);
