/* Alloy Loader - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

/* Global includes */
#include <libBareMetal.h>

/* Global functions */
unsigned long b_disk_read(void *mem, unsigned long start, unsigned long num, unsigned long disknum);

/* Global typedefs */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

int main()
{
	// Load the BMFS directory into memory
	
	// Does alloy.bin exist?
	
	// If so, load it to 0x200000 and jump to it
	
	// If not write an error to standard output
}

unsigned long b_disk_read(void *mem, unsigned long start, unsigned long num, unsigned long disknum) {
	unsigned long tlong;
	asm volatile ("call *0x00100060" : "=c"(tlong) : "a"(start), "c"(num), "d"(disknum), "D"(mem));
	return tlong;
}
