#!/bin/bash

set -e
set -u

CC=gcc
CFLAGS="-Wall -Wextra -Werror -Wfatal-errors -g -std=gnu99"

$CC $CFLAGS alloy-font-generator.c -o alloy-font-generator `pkg-config --cflags --libs freetype2`
