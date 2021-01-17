#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "Persist.h"
#include "ObjectTimer.h"
#include "Server.h"
#include "Database.h"
#include "Config.h"
#include "ValueObject.h"
#include "Timer.h"
#include "Hash.h"
#include "Set.h"
#include "LinkedList.h"
#include "Debug.h"

void saveHash(FILE* fp, struct Hash* phash)
{
    // hash size
    fwrite(&phash->size,sizeof(unsigned int), 1, fp);

    // 存储键值
    // (VALUE_TYPE_STR) nKey key type nValue value expire
    // (VALUE_TYPE_HASH) nKey key type num nKey1 key1 nValue value1 ... expire
    // (VALUE_TYPE_SET) nKey key type num nKey key1 [nKey2 key2 ...] expire
    // (VALUE_TYPE_LIST) nKey key type num nValue1 value1 [nValue2 value2 ...] expire
    char* key, *value;
    int nKey, nValue;
    struct ValueObject* pObj;
    while (iterateHash(phash, (void**)&key, (void**)&pObj))
    {
        // nKey
        nKey = strlen(key)+1;
        fwrite(&nKey, sizeof(unsigned int), 1, fp);

        // key
        fwrite(key, sizeof(char), nKey, fp);

        // type
        int type = pObj->type;
        fwrite(&type, sizeof(int), 1, fp);

        switch (pObj->type)
        {
            case VALUE_TYPE_STR:
            {
                value = pObj->value.str;
                nValue = strlen(value)+1;

                // nValue
                fwrite(&nValue, sizeof(int), 1, fp);

                // value
                fwrite(value, sizeof(char), nValue, fp);
                break;
            }

            case VALUE_TYPE_HASH:
            {
                // num
                int num = pObj->value.pHash->size;
                fwrite(&num, sizeof(int), 1, fp);

                while (iterateHash(pObj->value.pHash, (void**)&key, (void**)&value))
                {
                    nKey = strlen(key)+1, nValue = strlen(value)+1;

                    fwrite(&nKey, sizeof(unsigned int), 1, fp);
                    fwrite(key, sizeof(char), nKey, fp);

                    fwrite(&nValue, sizeof(int), 1, fp);
                    fwrite(value, sizeof(char), nValue, fp);
                }
                break;
            }

            case VALUE_TYPE_SET:
            {
                // num
                int num = sizeSet(pObj->value.pSet);
                fwrite(&num, sizeof(int), 1, fp);

                while (iterateSet(pObj->value.pSet, (void**)&key))
                {
                    nKey = strlen(key)+1;

                    fwrite(&nKey, sizeof(unsigned int), 1, fp);
                    fwrite(key, sizeof(char), nKey, fp);
                }
                break;
            }

            case VALUE_TYPE_LIST:
            {
                // num
                int num = getListSize(pObj->value.plist);
                fwrite(&num, sizeof(int), 1, fp);

                while (iterateList(pObj->value.plist, (void**)&key))
                {
                    nKey = strlen(key)+1;

                    fwrite(&nKey, sizeof(unsigned int), 1, fp);
                    fwrite(key, sizeof(char), nKey, fp);
                }
                break;
            }
                
            default:
                break;
        }

        // expire
        long when = -1;
        if (pObj->pTimer)
            when = pObj->pTimer->when;
        fwrite(&when, sizeof(long), 1, fp);
    }
}

struct Hash* loadHash(FILE* fp, struct DBServer* pServer, int index)
{
    struct Hash* phash = newHash();
    unsigned int size;
    fread(&size, sizeof(unsigned int), 1, fp);

    char key[512];
    struct ValueObject obj;
    for (unsigned int i = 0; i < size; ++i)
    {
        int nKey, nValue, type;
        fread(&nKey, sizeof(int), 1, fp);  
        fread(key, sizeof(char), nKey, fp);

        // type
        fread(&type, sizeof(int), 1, fp);

        switch (type)
        {
            case VALUE_TYPE_STR:
            {
                fread(&nValue, sizeof(int), 1, fp);
                char* tmpValue = (char*)malloc(sizeof(char)*nValue);
                fread(tmpValue, sizeof(char), nValue, fp);

                obj.type = type;
                obj.value.str = tmpValue;
                
                break;
            }

            case VALUE_TYPE_HASH:
            {
                // num
                int num;
                char tmpkey[512], tmpValue[512];
                int tmpnKey, tmpnValue;

                fread(&num, sizeof(int), 1, fp);

                struct Hash* ptmpHash = newHash();
                for (int j = 0; j < num; ++j)
                {
                    fread(&tmpnKey, sizeof(unsigned int), 1, fp);
                    fread(tmpkey, sizeof(char), tmpnKey, fp);

                    fread(&tmpnValue, sizeof(int), 1, fp);
                    fread(tmpValue, sizeof(char), tmpnValue, fp);
                    putKV(ptmpHash, tmpkey, tmpValue, tmpnValue);
                }

                obj.type = type;
                obj.value.pHash = ptmpHash;
                break;
            }

            case VALUE_TYPE_SET:
            {
                // num
                int num;
                char tmpkey[512];
                int tmpnKey;

                fread(&num, sizeof(int), 1, fp);

                struct Set* pSet = newSet();
                for (int j = 0; j < num; ++j)
                {
                    fread(&tmpnKey, sizeof(unsigned int), 1, fp);
                    fread(tmpkey, sizeof(char), tmpnKey, fp);
                    addSet(pSet, tmpkey);
                }

                obj.type = type;
                obj.value.pSet = pSet;
                break;
            }

            case VALUE_TYPE_LIST:
            {
                // num
                int num;
                char tmpkey[512];
                int tmpnKey;

                fread(&num, sizeof(int), 1, fp);

                struct LinkedList* plist = newLinkedList();
                for (int j = 0; j < num; ++j)
                {
                    fread(&tmpnKey, sizeof(unsigned int), 1, fp);
                    fread(tmpkey, sizeof(char), tmpnKey, fp);
                    rpush(plist, tmpkey, tmpnKey);
                }

                obj.type = type;
                obj.value.plist = plist;
                break;
            }
                
            default:
                break;
        }

        // expire
        long when;
        fread(&when, sizeof(long), 1, fp);
        if (when == -1)
            obj.pTimer = NULL;
        else 
        {
            struct Timer* pTimer = newObjectTimer(pServer,when, key, index);
            addTimer(pServer->pTimerQueue, pTimer);
            obj.pTimer = pTimer;
        }

        putKV(phash, key, &obj, sizeof(struct ValueObject));
    }

    return phash;
}

// 开启子进程进行持久化操作
void persist(struct DBServer* pServer)
{
    pid_t pid;
    pid = fork();

    // 忽略SIGCHLD信号，防止生成僵尸进程
    signal(SIGCHLD, SIG_IGN); 
    
    // 父进程退出
    if (pid > 0)
        return;

    // 子进程进行持久化操作
    FILE* fp;
    if ((fp = fopen(GET_CONFIG(pServer->pConfig, "dbfilename"), "wb")) == NULL)
    {
        perror("File open filed!");
        exit(0);
    }

    for (int i = 0; i < pServer->pDB->nDB; ++i)
    {
        saveHash(fp, pServer->pDB->phash[i]);
    }

    // 将用户缓冲区中的数据立即保存到内核缓冲区中
    if (fflush(fp) == EOF) 
    {
        perror("fflush error!");
        exit(0);
    }

    // 将内核缓冲区中的数据立即保存到硬盘中
    if (fsync(fileno(fp)) == -1)
    {
        perror("fsycn error!");
        exit(0);
    }

    fclose(fp);

    zmprintf("Saving database done.\n");

    // 子进程进行持久化操作后自我终止
    exit(0);  
}

void loadDB(struct DBServer* pServer)
{
    FILE* fp;
    int nDB = GET_CONFIG_UINT(pServer->pConfig, "numOfDB");
    if ((fp = fopen(GET_CONFIG(pServer->pConfig, "dbfilename"), "rb")) == NULL)
    {
        pServer->pDB = newDB(nDB);
        return;
    }

    pServer->pDB = (struct Database*)malloc(sizeof(struct Database));

    pServer->pDB->nDB = nDB;
    pServer->pDB->phash = (struct Hash**)malloc(sizeof(struct Hash*)*pServer->pDB->nDB);
    for (unsigned int i = 0; i < pServer->pDB->nDB; ++i)
    {
        pServer->pDB->phash[i] = loadHash(fp, pServer, i);
    }

    fclose(fp);
    zmprintf("Loding db done, nDB = %u\n", pServer->pDB->nDB);
}