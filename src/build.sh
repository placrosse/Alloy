#!/bin/bash

set -e

source "../bash/common.sh"

../utils/alloy-font-generator

CFLAGS="${CFLAGS} -fomit-frame-pointer"
CFLAGS="${CFLAGS} -fno-stack-protector"
CFLAGS="${CFLAGS} -mno-red-zone"

LDFLAGS="${LDFLAGS} -L ../lib"
LDLIBS="${LDLIBS} -lalloy"

OBJCOPY=${CROSS_COMPILE}objcopy
NASM=nasm

function convert_to_binary {
	echo " OBJCOPY ${PWD}/$2"
	$OBJCOPY -O binary $1 $2
}

compile_file alloy.c

if [ -z ${ALLOY_WITH_BAREMETAL+x} ]; then

	compile_file "xterm.c"
	compile_file "host-linux.c"

	link_executable "alloy" \
		"alloy.o"      \
		"host-linux.o" \
		"xterm.o"
else

	CFLAGS="${CFLAGS} -nostdinc"
	CFLAGS="${CFLAGS} -nodefaultlibs"
	CFLAGS="${CFLAGS} -DALLOY_WITH_BAREMETAL=1"
	CFLAGS="${CFLAGS} -O3"

	LDLIBS="${LDLIBS} ${BMFS_LIBRARY}"

	LDFLAGS="${LDFLAGS} -nostdlib"
	LDFLAGS="${LDFLAGS} -nostartfiles"
	LDFLAGS="${LDFLAGS} -T alloy.ld"

	compile_file "bmfs-host.c"
	compile_file "bmfs-disk.c"
	compile_file "font.c"
	compile_file "vesaterm.c"
	compile_file "host-baremetal.c"

	link_executable "alloy" \
		"alloy.o"      \
		"bmfs-host.o"  \
		"bmfs-disk.o"  \
		"font.o"       \
		"vesaterm.o"   \
		"host-baremetal.o"

	convert_to_binary "alloy" "alloy.bin"
fi

