#pragma once

struct Hash;
struct DBServer;
struct DBClient;

// 无序且不重复元素的集合
struct Set
{
    struct Hash* pHash;
};

struct Set* newSet();
int addSet(struct Set* pSet, const char* key);
int removeSet(struct Set* pSet, const char* key);
int sizeSet(struct Set* pSet);
int sismember(struct Set* pSet, const char* key);
void formatSet(struct Set* pSet, char* str);
struct Set* diffSet(struct Set* pSet1, struct Set* pSet2);  // 求两个集合的差集
struct Set* interSet(struct Set* pSet1, struct Set* pSet2);  // 求两个集合的交集
struct Set* interSets(struct Set** pSets, int nSets);  // 求多个集合的交集
struct Set* unionSet(struct Set* pSet1, struct Set* pSet2);  // 求两个集合的并集
struct Set* unionSets(struct Set** pSets, int nSets);  // 求多个集合的并集
int iterateSet(struct Set* pSet, void** pElement);
void freeSet(struct Set* pSet);