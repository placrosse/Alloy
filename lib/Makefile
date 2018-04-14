TOP ?= $(CURDIR)/..
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

VPATH += $(TOP)/include/alloy
VPATH += $(TOP)/utils

tests += input-test
tests += heap-test

targets += liballoy.a
targets += $(tests)

libfiles += cmd.o
libfiles += color.o
libfiles += heap.o
libfiles += host.o
libfiles += input.o
libfiles += shell.o
libfiles += term.o

.PHONY: all
all: $(targets)

liballoy.a: $(libfiles)

cmd.o: cmd.c cmd.h

color.o: color.c color.h types.h

heap.o: heap.c heap.h types.h errno.h

heap-test.o: heap-test.c heap.h types.h

heap-test: heap-test.o liballoy.a

host.o: host.c host.h types.h

input.o: input.c input.h

input-test.o: input-test.c input.h

input-test: input-test.o liballoy.a

shell.o: shell.c shell.h term.h input.h

term.o: term.c term.h

.PHONY: clean
clean:
	$(RM) $(libfiles)
	$(RM) $(tests) $(tests:=.o)
	$(RM) liballoy.a

.PHONY: test
test: $(tests)
	./heap-test
	./input-test

.PHONY: install
install: liballoy.a
	mkdir -p $(DESTDIR)$(PREFIX)/include/alloy
	cp ../include/alloy/*.h $(DESTDIR)$(PREFIX)/include/alloy
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp liballoy.a $(DESTDIR)$(PREFIX)/lib/liballoy.a

$(V).SILENT:
