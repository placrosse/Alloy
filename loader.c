/* Alloy Loader - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

/* Global functions */
unsigned long b_disk_read(void *mem, unsigned long start, unsigned long num, unsigned long disknum);
int int_strncmp(const char *str1, const char *str2, unsigned int num);

/* Global typedefs */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

/* Global defines*/
struct BMFSEntry
{
	char FileName[32];
	u64 StartingBlock;
	u64 ReservedBlocks;
	u64 FileSize;
	u64 Unused;
};

/* Global variables */
unsigned long AlloyStartBlock;
unsigned long AlloyLength;
struct BMFSEntry *BMFS_Dir;

int main()
{
	BMFS_Dir = (void*)0x100000;
	// Load the BMFS directory into memory
	b_disk_read(BMFS_Dir, 1, 1, 0); // Copy second 4K sector to memory
	
	// Does alloy.bin exist?
	
	
	// If so, load it to 0x200000 and jump to it
	
	// If not write an error to standard output

	return 0;
}

unsigned long b_disk_read(void *mem, unsigned long start, unsigned long num, unsigned long disknum) {
	unsigned long tlong;
	asm volatile ("call *0x00100060" : "=c"(tlong) : "a"(start), "c"(num), "d"(disknum), "D"(mem));
	return tlong;
}

int int_strncmp(const char *str1, const char *str2, unsigned int num)
{
	while(num--)
		if(*str1++ != *str2++)
			return *(unsigned char*)(str1-1) - *(unsigned char*)(str2-1);
	return 0;
}
