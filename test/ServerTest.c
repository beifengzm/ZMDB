#include "Server.h"
#include "Config.h"
#include "Debug.h"

#include <stdlib.h>
#include <stdio.h>

int main()
{
    struct DBServer* pServer = newServer("../zmdb.conf");
    pServer->loop(pServer);

    return 0;
}