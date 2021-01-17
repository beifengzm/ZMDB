#pragma once

struct DBServer;
struct DBClient;
struct ValueObject;

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