#include <string.h>

char *dirname(char *path)
{
    char *p;

    if (!path || *path == '\0')
        return ".";
    p = path + strlen(path) - 1;
    while (p > path && *p == '/')
        p--;
    while (p > path && *p != '/')
        p--;
    if (*p != '/')
        return ".";
    while (p > path && *p == '/')
        p--;
    p[1] = '\0';
    return path;
}
