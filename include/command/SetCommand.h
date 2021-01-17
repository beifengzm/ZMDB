#pragma once

struct Set;
struct DBServer;
struct DBClient;

/********************  Set Command  ********************/
void saddCommand(struct DBServer* pServer, struct DBClient* pClient);
void scardCommand(struct DBServer* pServer, struct DBClient* pClient);  // 获取集合的元素个数
void sdiffCommand(struct DBServer* pServer, struct DBClient* pClient);
void sismemberCommand(struct DBServer* pServer, struct DBClient* pClient);
void sinterCommand(struct DBServer* pServer, struct DBClient* pClient);
void sunionCommand(struct DBServer* pServer, struct DBClient* pClient);
void sremoveCommand(struct DBServer* pServer, struct DBClient* pClient);