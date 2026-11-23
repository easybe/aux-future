# Builds libfuture.a with the A/UX cross compiler:
#
#     make
#     make install PREFIX=/opt/aux/m68k-apple-aux
#
# The headers go to $(PREFIX)/include/future, the library to
# $(PREFIX)/lib.

CC = m68k-apple-aux-gcc
AR = m68k-apple-aux-ar
RANLIB = m68k-apple-aux-ranlib
CFLAGS = -O2 -Wall
CPPFLAGS = -D_POSIX_SOURCE -Iinclude -include include/future.h

PREFIX = /usr/local
DESTDIR =

HEADERS = $(wildcard include/*.h)
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

all: libfuture.a

libfuture.a: $(OBJS)
	rm -f $@
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

$(OBJS): $(HEADERS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

install: libfuture.a
	mkdir -p $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include/future
	cp libfuture.a $(DESTDIR)$(PREFIX)/lib/
	cp $(HEADERS) $(DESTDIR)$(PREFIX)/include/future/

clean:
	rm -f libfuture.a $(OBJS)

.PHONY: all install clean
