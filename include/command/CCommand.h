#pragma once

struct DBServer;
struct DBClient;
struct ValueObject;

// 删除旧的键值对(如果存在)
void removeOldMapIfExist(struct DBServer* pServer, struct DBClient* pClient);
struct ValueObject* getObjectAndCheck(struct DBServer* pServer, struct DBClient* pClient);

/********************  Common command  ********************/
void delCommand(struct DBServer* pServer, struct DBClient* pClient);
void keysCommand(struct DBServer* pServer, struct DBClient* pClient);
void getCommand(struct DBServer* pServer, struct DBClient* pClient);
void enterCommand(struct DBServer* pServer, struct DBClient* pClient);
void persistCommand(struct DBServer* pServer, struct DBClient* pClient);
void mexpireCommand(struct DBServer* pServer, struct DBClient* pClient);
void expireCommand(struct DBServer* pServer, struct DBClient* pClient);
void ttlCommand(struct DBServer* pServer, struct DBClient* pClient);
void ttlsCommand(struct DBServer* pServer, struct DBClient* pClient);