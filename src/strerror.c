#include <stdio.h>

extern char *sys_errlist[];
extern int sys_nerr;

char *strerror(int errnum)
{
    static char unknown[32];

    if (errnum >= 0 && errnum < sys_nerr)
        return sys_errlist[errnum];
    sprintf(unknown, "Unknown error %d", errnum);
    return unknown;
}
