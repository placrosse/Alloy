#!/bin/bash

set -e

source "../bash/common.sh"

CFLAGS="${CFLAGS} -fno-stack-protector"
CFLAGS="${CFLAGS} -fomit-frame-pointer"
CFLAGS="${CFLAGS} -mno-red-zone"

compile_file "app.c"
compile_file "app-bin.c"
compile_file "app-elf.c"
compile_file "cmd.c"
compile_file "color.c"
compile_file "heap.c"
compile_file "host.c"
compile_file "input.c"
compile_file "path.c"
compile_file "scanner.c"
compile_file "scheme.c"
compile_file "shell.c"
compile_file "string.c"
compile_file "term.c"
compile_file "var.c"

link_static "liballoy.a" \
	"app.o" \
	"app-bin.o" \
	"app-elf.o" \
	"cmd.o" \
	"color.o" \
	"heap.o" \
	"host.o" \
	"input.o" \
	"path.o" \
	"scanner.o" \
	"scheme.o" \
	"shell.o" \
	"string.o" \
	"term.o" \
	"var.o"
