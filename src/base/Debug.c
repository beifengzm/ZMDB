#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "Debug.h"

void zmprintf(char *format, ...)
{
    time_t timep;
    struct tm *p;
    time (&timep);
    p=gmtime(&timep);
    char buff[512];
    sprintf(buff, "[%d-%02d-%02d %02d:%02d:%02d] %s", 1900+p->tm_year,1+p->tm_mon,
        p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec, format);

    va_list args;
    va_start(args, format);
    vprintf(buff, args);
    va_end(args);
}