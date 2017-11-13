include ../make/patterns.mk

CC ?= $(CROSS_COMPILE)gcc
CFLAGS := $(CFLAGS) -Wall -Wextra -Werror -Wfatal-errors -std=gnu99 -g
CFLAGS := $(CFLAGS) -I ../include

ifdef ALLOY_WITH_BAREMETAL
TOP ?= ../../..
CFLAGS := $(CFLAGS) -fno-stack-protector -fomit-frame-pointer -mno-red-zone
CFLAGS := $(CFLAGS) -DALLOY_WITH_BAREMETAL=1
CFLAGS := $(CFLAGS) -I $(TOP)/output/include
LD = $(CROSS_COMPILE)ld
LDFLAGS := $(LDFLAGS) -static -L $(TOP)/output/lib
LDLIBS := $(LDLIBS) -lbmfs -lc
targets += alloy.bin
else
LD = $(CROSS_COMPILE)gcc
tests += vesaterm-test
tests += input-test
endif

LD ?= $(CROSS_COMPILE)ld
LDFLAGS ?=
LDLIBS ?=

AR ?= $(CROSS_COMPILE)ar
ARFLAGS = rcs

VPATH += ../include/alloy
VPATH += ../utils

targets += liballoy.a
targets += $(tests)

libfiles += font.o
libfiles += input.o
libfiles += shell.o
libfiles += term.o
libfiles += vesaterm.o

.PHONY: all
all: $(targets)

alloy.bin: alloy

alloy: alloy.o liballoy.a

alloy.o: alloy.c vesaterm.h term.h

liballoy.a: $(libfiles)

font.o: font.c font.h

font.c: alloy-font-generator
	./$<

input.o: input.c input.h

input-test.o: input-test.c input.h

input-test: input-test.o liballoy.a

shell.o: shell.c shell.h term.h input.h

term.o: term.c term.h

vesaterm.o: vesaterm.c vesaterm.h term.h

vesaterm-test.o: vesaterm-test.c vesaterm.h term.h

vesaterm-test: vesaterm-test.o liballoy.a

.PHONY: clean
clean:
	$(RM) font.c
	$(RM) $(libfiles)
	$(RM) $(tests) $(tests:=.o)
	$(RM) liballoy.a

.PHONY: test
test: $(tests)
	./vesaterm-test
	./input-test

$(V).SILENT: