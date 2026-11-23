#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return len;
}
