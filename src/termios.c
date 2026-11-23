/*
 * Serial port extensions for the A/UX serial driver.
 *
 * The driver accepts UIOCMODEM, UIOCNOMODEM and UIOCDTRFLOW, but not
 * UIOCFLOW and UIOCNOFLOW. In the UIOCDTRFLOW mode, it stops sending
 * while the handshake input (HSKi) is negated. It does not drive the
 * handshake output for input flow control. CRTSCTS selects UIOCDTRFLOW,
 * and clearing it selects UIOCNOMODEM. The driver keeps the mode after
 * the device is closed.
 *
 * TCSETBAUD sets the SCC time constant to 115200 / rate - 2. B57600 is
 * stored as B38400 plus a c_cflag bit, and tcsetattr() applies it with
 * TCSETBAUD. 115200 is not possible.
 *
 * The driver asserts DTR whenever it applies the termios settings.
 * tcsetattr() clears DTR again if it was lowered with TIOCMBIC.
 *
 * The kernel does not store the extra c_cflag bits. tcgetattr() reads
 * the flow control mode from the driver and the B57600 setting from a
 * per-fd table that only covers this process.
 */
#include <errno.h>
#include <stdarg.h>

#undef tcgetattr
#undef tcsetattr
#undef cfsetospeed
#undef cfsetispeed
#undef cfgetospeed
#undef cfgetispeed
#undef ioctl

#include <termios.h>
#include <sys/ioctl.h>

#include "tiocm.h"

#undef ioctl

#define FAST 0x20000000UL
#define MAXFD 64

static char fast[MAXFD];
static char dtrdown[MAXFD];

/*
 * Returns 1 if the device is in the UIOCDTRFLOW mode, 0 if not, and -1
 * if it has no modem control, such as a pty.
 */
static int dtrflow(int fd)
{
    char st[3];

    if (ioctl(fd, UIOCTTSTAT, st) < 0)
        return -1;
    return st[1] != 0;
}

static int setflow(int fd, int flow)
{
    int cur = dtrflow(fd);

    if (cur < 0) {
        if (!flow)
            return 0;
        errno = EINVAL;
        return -1;
    }
    if (flow == cur)
        return 0;
    return ioctl(fd, flow ? UIOCDTRFLOW : UIOCNOMODEM, 0);
}

int future_tcgetattr(int fd, struct termios *tio)
{
    if (tcgetattr(fd, tio) < 0)
        return -1;
    tio->c_cflag &= ~(CRTSCTS | FAST);
    if (dtrflow(fd) == 1)
        tio->c_cflag |= CRTSCTS;
    if (fd >= 0 && fd < MAXFD && fast[fd] &&
        (tio->c_cflag & CBAUD) == B38400)
        tio->c_cflag |= FAST;
    return 0;
}

int future_tcsetattr(int fd, int action, const struct termios *tio)
{
    struct termios t = *tio;
    int isfast = (tio->c_cflag & FAST) != 0;
    unsigned short rate = 57600;

    t.c_cflag &= ~(CRTSCTS | FAST);
    if (tcsetattr(fd, action, &t) < 0)
        return -1;
    if (isfast && ioctl(fd, TCSETBAUD, &rate) < 0)
        return -1;
    if (fd >= 0 && fd < MAXFD) {
        fast[fd] = isfast;
        if (dtrdown[fd] && ioctl(fd, TCCLRDTR, 0) < 0)
            return -1;
    }
    return setflow(fd, (tio->c_cflag & CRTSCTS) != 0);
}

/* A/UX has one speed for both directions. B0 as input speed keeps it. */
static int setspeed(struct termios *tio, unsigned int speed, int out)
{
    if (speed == B57600) {
        tio->c_cflag |= FAST;
        speed = B38400;
    } else if (out || speed != B0) {
        tio->c_cflag &= ~FAST;
    }
    return out ? cfsetospeed(tio, speed) : cfsetispeed(tio, speed);
}

int future_cfsetospeed(struct termios *tio, unsigned int speed)
{
    return setspeed(tio, speed, 1);
}

int future_cfsetispeed(struct termios *tio, unsigned int speed)
{
    return setspeed(tio, speed, 0);
}

unsigned int future_cfgetospeed(const struct termios *tio)
{
    if (tio->c_cflag & FAST)
        return B57600;
    return cfgetospeed(tio);
}

unsigned int future_cfgetispeed(const struct termios *tio)
{
    if (tio->c_cflag & FAST)
        return B57600;
    return cfgetispeed(tio);
}

/*
 * The driver always asserts RTS. The Mac serial ports have no RTS pin,
 * so RTS is reported as set and cannot be cleared. The DTR state is the
 * one last set by this process, not read back from the SCC.
 */
static int modemctl(int fd, int req, int *bits)
{
    struct serstat st;
    int up;

    if (req == TIOCMGET) {
        if (ioctl(fd, TCGETSTAT, &st) < 0)
            return -1;
        *bits = TIOCM_RTS;
        if (fd < 0 || fd >= MAXFD || !dtrdown[fd])
            *bits |= TIOCM_DTR;
        if (st.ser_cts)
            *bits |= TIOCM_CTS;
        return 0;
    }
    if (req == TIOCMBIC && (*bits & TIOCM_RTS)) {
        errno = EINVAL;
        return -1;
    }
    if (!(*bits & TIOCM_DTR))
        return 0;
    up = req == TIOCMBIS;
    if (ioctl(fd, up ? TCSETDTR : TCCLRDTR, 0) < 0)
        return -1;
    if (fd >= 0 && fd < MAXFD)
        dtrdown[fd] = !up;
    return 0;
}

int future_ioctl(int fd, int req, ...)
{
    va_list ap;
    char *arg;

    va_start(ap, req);
    arg = va_arg(ap, char *);
    va_end(ap);
    if (req == TIOCMGET || req == TIOCMBIS || req == TIOCMBIC)
        return modemctl(fd, req, (int *)arg);
    return ioctl(fd, req, arg);
}
