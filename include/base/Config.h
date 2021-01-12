#pragma once

struct Hash;
struct DBConfig
{
    struct Hash* phash;
};

#define GET_CONFIG(pConfig, key) (char*)getConfig(pConfig, key)
#define GET_CONFIG_USHORT(pConfig, key) (unsigned short)atoi(GET_CONFIG(pConfig, key))
#define GET_CONFIG_INT(pConfig, key) (int)atoi(GET_CONFIG(pConfig, key))
#define GET_CONFIG_UINT(pConfig, key) (unsigned int)atoi(GET_CONFIG(pConfig, key))

struct DBConfig* newConfig(const char* filename);
char* getConfig(const struct DBConfig* pConfig, const void* key);
void freeConfig(struct DBConfig* pConfig);
