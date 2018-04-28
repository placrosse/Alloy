#!/bin/bash

set -e

source ../bash/common.sh

CFLAGS="${CFLAGS} `pkg-config --cflags freetype2`"
compile_file alloy-font-generator.c

LDLIBS="${LDLIBS} `pkg-config --libs freetype2`"
link_executable "alloy-font-generator" \
	"alloy-font-generator.o"
