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

Headers that A/UX does not have: `stdint.h`, `getopt.h`, `libgen.h`.

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
