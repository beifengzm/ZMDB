#pragma once

struct DBServer;

typedef void (*Select_loop)(struct DBServer*);
Select_loop getSelectLoop();