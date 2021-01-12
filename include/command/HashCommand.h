#pragma once

struct DBServer;
struct DBClient;

void hsetCommand(struct DBServer* pServer, struct DBClient* pClient);