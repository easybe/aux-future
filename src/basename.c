#include <string.h>

char *basename(char *path)
{
    char *p;

    if (!path || *path == '\0')
        return ".";
    p = path + strlen(path) - 1;
    while (p > path && *p == '/')
        *p-- = '\0';
    if (p == path && *p == '/')
        return path;
    p = strrchr(path, '/');
    return p ? p + 1 : path;
}
