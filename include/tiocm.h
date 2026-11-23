/*
 * The TIOCMGET, TIOCMBIS and TIOCMBIC ioctls for the A/UX serial driver.
 * See src/termios.c. Include this file explicitly:
 *
 *     #include <future/tiocm.h>
 *
 * It replaces ioctl() with a wrapper that handles these requests and
 * passes all others on.
 */
#ifndef FUTURE_TIOCM_H
#define FUTURE_TIOCM_H

#include <sys/ioctl.h>

#define TIOCMGET _IOR('p', 1, int)
#define TIOCMBIS _IOW('p', 2, int)
#define TIOCMBIC _IOW('p', 3, int)

#define TIOCM_DTR 0x002
#define TIOCM_RTS 0x004
#define TIOCM_CTS 0x020
#define TIOCM_CD 0x040
#define TIOCM_RI 0x080
#define TIOCM_DSR 0x100

int future_ioctl(int fd, int req, ...);

#define ioctl future_ioctl

#endif
