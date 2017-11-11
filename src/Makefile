CC ?= $(CROSS_COMPILE)gcc
CFLAGS := $(CFLAGS) -Wall -Wextra -Werror -Wfatal-errors -std=gnu99
CFLAGS := $(CFLAGS) -I ../include

LD ?= $(CROSS_COMPILE)ld
LDFLAGS ?=
LDLIBS ?=

AR ?= $(CROSS_COMPILE)ar
ARFLAGS = rcs

VPATH += ../include/alloy
VPATH += ../utils

libfiles += font.o
libfiles += term.o
libfiles += vesaterm.o

.PHONY: all
all: liballoy.a

liballoy.a: $(libfiles)
	$(AR) $(ARFLAGS) $@ $(libfiles)

font.o: font.c font.h

font.c: alloy-font-generator
	./$<

term.o: term.c term.h

vesaterm.o: vesaterm.c vesaterm.h term.h

.PHONY: clean
clean:
	$(RM) font.c
	$(RM) $(libfiles)
	$(RM) liballoy.a
