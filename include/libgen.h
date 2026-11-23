/*
 * basename() and dirname() as in POSIX. Both may modify the argument.
 */
#ifndef FUTURE_LIBGEN_H
#define FUTURE_LIBGEN_H

char *basename(char *path);
char *dirname(char *path);

#endif
