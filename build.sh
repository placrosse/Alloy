#!/bin/sh
gcc -c -m64 -Wall -W -pedantic -std=c99 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o alloy.o alloy.c
gcc -c -m64 -Wall -W -pedantic -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o libBareMetal.o libBareMetal.c
ld -T app.ld -o alloy.app alloy.o libBareMetal.o
