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

libfiles += app.o
libfiles += app-bin.o
libfiles += app-elf.o
libfiles += cmd.o
libfiles += color.o
libfiles += heap.o
libfiles += host.o
libfiles += input.o
libfiles += scanner.o
libfiles += scheme.o
libfiles += shell.o
libfiles += string.o
libfiles += term.o
libfiles += var.o

.PHONY: all
all: $(targets)

liballoy.a: $(libfiles)

app.o: app.c app.h types.h errno.h

app-bin.o: app-bin.c app.h types.h errno.h

app-elf.o: app-elf.c app.h types.h errno.h

cmd.o: cmd.c cmd.h

color.o: color.c color.h types.h

heap.o: heap.c heap.h types.h errno.h

heap-test.o: heap-test.c heap.h types.h

heap-test: heap-test.o liballoy.a

host.o: host.c host.h types.h

input.o: input.c input.h

input-test.o: input-test.c input.h

input-test: input-test.o liballoy.a

scanner.o: scanner.c scanner.h types.h errno.h

scheme.o: scheme.c scheme.h color.h types.h

shell.o: shell.c shell.h term.h color.h scanner.h host.h heap.h cmd.h app.h string.h input.h

string.o: string.c string.h types.h

term.o: term.c term.h

var.o: var.c var.h types.h errno.h scanner.h

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
