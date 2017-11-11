CC ?= $(CROSS_COMPILE)gcc
CFLAGS := $(CFLAGS) -Wall -Wextra -Werror -Wfatal-errors -std=gnu99 -g
CFLAGS := $(CFLAGS) -I ../include
ifdef ALLOY_WITH_BAREMETAL
CFLAGS := $(CFLAGS) -DALLOY_WITH_BAREMETAL=1
endif

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

tests += vesaterm-test

.PHONY: all
all: liballoy.a $(tests)

liballoy.a: $(libfiles)
	$(AR) $(ARFLAGS) $@ $(libfiles)

font.o: font.c font.h

font.c: alloy-font-generator
	./$<

term.o: term.c term.h

vesaterm.o: vesaterm.c vesaterm.h term.h

vesaterm-test.o: vesaterm-test.c vesaterm.h term.h

vesaterm-test: vesaterm-test.o liballoy.a

.PHONY: clean
clean:
	$(RM) font.c
	$(RM) $(libfiles)
	$(RM) liballoy.a

.PHONY: test
test: $(tests)
	./vesaterm-test
