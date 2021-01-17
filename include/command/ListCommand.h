#pragma once

struct LinkedList;
struct DBServer;
struct DBClient;

/********************  LinkedList Command  ********************/
void lpushCommand(struct DBServer* pServer, struct DBClient* pClient);
void rpushCommand(struct DBServer* pServer, struct DBClient* pClient);
void lindexCommand(struct DBServer* pServer, struct DBClient* pClient);
void lsizeCommand(struct DBServer* pServer, struct DBClient* pClient);
void lpopCommand(struct DBServer* pServer, struct DBClient* pClient);
void rpopCommand(struct DBServer* pServer, struct DBClient* pClient);
void lrangeCommand(struct DBServer* pServer, struct DBClient* pClient);