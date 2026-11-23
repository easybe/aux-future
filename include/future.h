/*
 * Declarations that the A/UX 3.1 system headers do not provide. Force
 * this file into every compilation unit:
 *
 *     -I<prefix>/include/future -include future.h
 *
 * The functions in the first block exist in the A/UX libc but have no
 * declaration. They are old-style declarations without parameter
 * lists, which stay compatible with the prototypes that configure
 * tests declare on their own. All arguments are ints or pointers, so
 * no default argument promotion changes them.
 */
#ifndef FUTURE_H
#define FUTURE_H

#include <stdarg.h>
#include <stddef.h>

int socket();
int socketpair();
int bind();
int connect();
int listen();
int accept();
int shutdown();
int getsockopt();
int setsockopt();
int getsockname();
int getpeername();
int recv();
int recvfrom();
int send();
int sendto();
int gethostname();
int select();
int gettimeofday();
int ioctl();
int ftruncate();
int fchmod();
int utimes();
int flock();
int strcasecmp();
int strncasecmp();
void bzero();

/* A/UX has no socklen_t. The socket calls take an int pointer. */
typedef int socklen_t;

/* Implemented in libfuture. */
char *strdup(const char *s);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
void *memmove(void *dst, const void *src, size_t n);
char *strerror(int errnum);
int usleep(unsigned long usec);

struct termios;
void cfmakeraw(struct termios *tio);

struct timespec {
    long tv_sec;
    long tv_nsec;
};

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
int clock_gettime(int clock_id, struct timespec *ts);

#endif
