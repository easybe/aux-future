#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/*
 * The A/UX headers declare strtoul(), but the libc lacks it.
 */
unsigned long strtoul(const char *nptr, char **endptr, int base)
{
    const char *s = nptr;
    unsigned long acc = 0, cutoff;
    int c, cutlim, neg = 0, any = 0, overflow = 0;

    while (isspace((unsigned char)*s))
        s++;
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    if ((base == 0 || base == 16) && s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X') && isxdigit((unsigned char)s[2])) {
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = s[0] == '0' ? 8 : 10;
    }
    if (base < 2 || base > 36) {
        errno = EINVAL;
        if (endptr)
            *endptr = (char *)nptr;
        return 0;
    }

    cutoff = ULONG_MAX / base;
    cutlim = (int)(ULONG_MAX % base);
    for (;; s++) {
        c = (unsigned char)*s;
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c = tolower(c) - 'a' + 10;
        else
            break;
        if (c >= base)
            break;
        any = 1;
        if (acc > cutoff || (acc == cutoff && c > cutlim))
            overflow = 1;
        else
            acc = acc * base + c;
    }

    if (endptr)
        *endptr = (char *)(any ? s : nptr);
    if (overflow) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    return neg ? -acc : acc;
}
