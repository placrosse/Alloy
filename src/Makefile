TOP ?= $(CURDIR)/..
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

BMFS_INCLUDE_DIR ?= /opt/return-infinity/include

BMFS_LIB_DIR ?= /opt/return-infinity/lib

CFLAGS += -I $(BMFS_INCLUDE_DIR)

LDFLAGS += -L $(BMFS_LIB_DIR)

VPATH += $(TOP)/include/alloy
VPATH += $(TOP)/utils
VPATH += $(BMFS_LIB_DIR)

targets += alloy
targets += liballoy.a
targets += $(tests)

libfiles += cmd.o
libfiles += color.o
libfiles += font.o
libfiles += host.o
libfiles += input.o
libfiles += shell.o
libfiles += term.o

ifdef PLATFORM_baremetal
libfiles += vesaterm.o
installfiles += $(DESTDIR)$(PREFIX)/system/loader.bin
installfiles += $(DESTDIR)$(PREFIX)/system/alloy.elf
installfiles += $(DESTDIR)$(PREFIX)/system/alloy.bin
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/vesaterm.h
endif

ifdef PLATFORM_linux
libfiles += xterm.o
endif

installfiles += $(DESTDIR)$(PREFIX)/lib/liballoy.a
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/error.h
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/font.h
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/input.h
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/shell.h
installfiles += $(DESTDIR)$(PREFIX)/include/alloy/term.h

.PHONY: all
all: $(targets)

loader.bin: loader.asm
	$(NASM) $< -o $@

loader.elf: loader.o

alloy: alloy.o liballoy.a -lbmfs

alloy.bin: alloy.elf

alloy.elf: alloy.o liballoy.a syscall-hooks.o -lbmfs -lc

alloy.elf: LDFLAGS+=-T alloy.ld

alloy.o: alloy.c term.h shell.h input.h errno.h types.h

liballoy.a: $(libfiles)

syscall-hooks.o: syscall-hooks.asm

cmd.o: cmd.c cmd.h

color.o: color.c color.h types.h

font.o: font.c font.h

font.c: alloy-font-generator
	$<

host.o: host.c host.h types.h

input.o: input.c input.h

input-test.o: input-test.c input.h

input-test: input-test.o liballoy.a

sdl-term.o: sdl-term.c sdl-term.h term.h

shell.o: shell.c shell.h term.h input.h

term.o: term.c term.h

vesaterm.o: vesaterm.c vesaterm.h term.h

vesaterm-test.o: vesaterm-test.c vesaterm.h term.h

vesaterm-test: vesaterm-test.o liballoy.a

xterm.o: xterm.c xterm.h types.h errno.h

.PHONY: clean
clean:
	$(RM) font.c
	$(RM) $(libfiles)
	$(RM) $(tests) $(tests:=.o)
	$(RM) liballoy.a
	$(RM) alloy.bin alloy.elf
	$(RM) loader.bin

.PHONY: test
test: $(tests)
	./vesaterm-test
	./input-test

.PHONY: install
install: loader.bin liballoy.a alloy.bin alloy.elf
	mkdir -p $(DESTDIR)$(PREFIX)/include/alloy
	cp ../include/alloy/*.h $(DESTDIR)$(PREFIX)/include/alloy
	cp liballoy.a $(DESTDIR)$(PREFIX)/lib/liballoy.a
	cp loader.bin $(DESTDIR)$(PREFIX)/system
	cp alloy.bin $(DESTDIR)$(PREFIX)/system
	cp alloy.elf $(DESTDIR)$(PREFIX)/system

$(V).SILENT:
