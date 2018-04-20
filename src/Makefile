TOP ?= $(CURDIR)/..
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

VPATH += $(TOP)/include/alloy
VPATH += $(TOP)/utils
VPATH += $(TOP)/lib
VPATH += $(PREFIX)/lib

ifdef PLATFORM_linux
objfiles += xterm.o
objfiles += host-linux.o
endif

ifdef PLATFORM_baremetal
objfiles += font.o
objfiles += vesaterm.o
objfiles += host-baremetal.o
LDFLAGS += -T alloy.ld
LDFLAGS += -nostdlib
targets += alloy.bin
endif

targets += alloy

.PHONY: all
all: $(targets)

alloy: alloy.o $(objfiles) liballoy.a -lbmfs

alloy.bin: alloy

alloy.o: alloy.c term.h shell.h input.h errno.h types.h

syscall-hooks.o: syscall-hooks.asm

font.o: font.c font.h

font.c: alloy-font-generator
	$<

host-linux.o: host-linux.c ./host.h $(TOP)/include/alloy/host.h types.h errno.h

sdl-term.o: sdl-term.c ./term.h $(TOP)/include/alloy/term.h types.h errno.h

vesaterm.o: vesaterm.c ./term.h $(TOP)/include/alloy/term.h types.h errno.h

vesaterm-test.o: vesaterm-test.c ./term.h $(TOP)/include/alloy/term.h types.h errno.h

vesaterm-test: vesaterm-test.o liballoy.a

xterm.o: xterm.c ./term.h $(TOP)/include/alloy/term.h types.h errno.h

.PHONY: clean
clean:
	$(RM) font.c
	$(RM) alloy
	$(RM) *.o

.PHONY: test
test:

ifdef PLATFORM_baremetal

.PHONY: install
install: alloy.bin
	cp alloy.bin $(PREFIX)/system

else

.PHONY: install
install: alloy
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp alloy $(DESTDIR)$(PREFIX)/bin/alloy

endif

$(V).SILENT:
