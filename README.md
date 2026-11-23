# aux-future

libfuture adds C library functions and headers that A/UX 3.1 lacks, for
porting current software with the `m68k-apple-aux` cross compiler
(https://github.com/easybe/gcc-aux, branch `aux`).

The library is static. Each program links only the functions it uses,
so nothing needs to be installed in the guest at runtime.

## Contents

`future.h` is meant to be forced into every compilation unit. It
declares functions that the A/UX libc has but its headers do not
declare (socket calls, `select`, `ioctl`, `strcasecmp`, `flock` and
others), adds `socklen_t` and `struct timespec`, and declares the
functions below.

Functions in `libfuture.a`:

- `strdup`, `snprintf`, `vsnprintf`, `memmove`, `strerror`, `strtoul`
- `getopt`, `getopt_long` (GNU style, with argument permutation)
- `basename`, `dirname`
- `usleep`, `clock_gettime`, `cfmakeraw`
- Wrappers for `tcgetattr`, `tcsetattr`, `cfsetospeed`, `cfsetispeed`,
  `cfgetospeed` and `cfgetispeed`. They add `CRTSCTS` and `B57600` for
  the serial driver. `future.h` maps the names to the wrappers.

Headers that A/UX does not have: `stdint.h`, `getopt.h`, `libgen.h`.

`<future/tiocm.h>` adds the `TIOCMGET`, `TIOCMBIS` and `TIOCMBIC`
ioctls for DTR and CTS. It replaces `ioctl()` with a wrapper, so it is
not part of `future.h`. Include it where the program uses these ioctls.

## Serial ports

The A/UX serial driver differs from Linux and BSD:

- `CRTSCTS` selects the `UIOCDTRFLOW` mode. The driver stops sending
  while the handshake input (HSKi) is negated. It does not drive the
  handshake output, so there is no input flow control.
- `B57600` uses the `TCSETBAUD` ioctl. 115200 is not possible.
- The Mac serial ports have no RTS pin. RTS reads as set and cannot be
  cleared.
- The driver asserts DTR whenever it applies the termios settings.
  `tcsetattr()` clears it again if it was lowered with `TIOCMBIC`.
- The kernel does not store `CRTSCTS` and `B57600` in the termios
  settings. `tcgetattr()` reads the flow control mode from the driver,
  but reports `B57600` only if the same process set it.

## Building

    make
    make install PREFIX=/opt/aux/m68k-apple-aux

This installs the headers to `$(PREFIX)/include/future` and the library
to `$(PREFIX)/lib`. `DESTDIR` is supported.

## Usage

    CPPFLAGS="-D_POSIX_SOURCE -I/opt/aux/m68k-apple-aux/include/future -include future.h"
    LIBS="-lfuture -lposix"

`-D_POSIX_SOURCE` makes the A/UX headers declare `read()`, `open()`,
`stat()` and `O_NONBLOCK`. It needs `-lposix`.

In the guest, the library works with gcc. The native cc is a K&R
compiler and rejects the prototypes in the headers.
