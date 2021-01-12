#pragma once

struct Hash;

struct Database
{
    unsigned int nDB;
    struct Hash** phash;
};

struct Database* newDB(unsigned int);
void closeDB(struct Database*);

// 从数据库中删除键值对
void removeMapFromDB(struct Database* pDB, const char* key, int index);
void removeMapFromDBIfExist(struct Database* pDB, const char* key, int index);