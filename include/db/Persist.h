#pragma once

#include <stdio.h>

struct DBServer;
struct Hash;

void saveHash(FILE* fp, struct Hash* pHash);
struct Hash* loadHash(FILE* fp, struct DBServer* pServer, int index);
void persist(struct DBServer* pServer);
void loadDB(struct DBServer* pServer);