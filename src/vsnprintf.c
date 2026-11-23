#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The output is first written to /dev/null to get its length. It is
 * formatted directly into the caller's buffer if it fits, and into a
 * temporary buffer otherwise. The result is truncated to the caller's
 * size like C99 vsnprintf() does.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    static FILE *null;
    va_list aq;
    char *tmp;
    int len;

    if (null == NULL && (null = fopen("/dev/null", "w")) == NULL)
        return -1;
    __va_copy(aq, ap);
    len = vfprintf(null, fmt, aq);
    va_end(aq);
    if (len < 0 || size == 0)
        return len;

    if ((size_t)len < size)
        return vsprintf(buf, fmt, ap);

    tmp = malloc((size_t)len + 1);
    if (tmp == NULL)
        return -1;
    vsprintf(tmp, fmt, ap);
    memcpy(buf, tmp, size - 1);
    buf[size - 1] = '\0';
    free(tmp);
    return len;
}
