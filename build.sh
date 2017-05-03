#!/bin/sh
gcc -c -m64 -Wall -W -pedantic -std=c99 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o alloy.o alloy.c
gcc -c -m64 -Wall -W -pedantic -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o libBareMetal.o libBareMetal.c
objcopy --remove-section .comment alloy.o
objcopy --remove-section .eh_frame alloy.o
objcopy --remove-section .comment libBareMetal.o
objcopy --remove-section .eh_frame libBareMetal.o
ld -T app.ld -o alloy.bin alloy.o libBareMetal.o
