/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "host.h"

#include <alloy/heap.h>

#include <baremetal/syscalls.h>

struct AlloyHostData
{
	/** The number of bytes in one page. */
	alloy_size page_size;
	/** Used to allocate directory structures and such. */
	struct AlloyHeap heap;
	/** A pointer to the heap buffer. */
	void *heap_data;
	/** The number of bytes in the heap. */
	alloy_size heap_size;
};

struct AlloyDir
{
	/** Not used, here to avoid memory
	 * allocations of zero bytes. */
	void *unused;
};

struct AlloyHostData global_host_data;

static struct AlloyHostData *host_init(void)
{
	/* TODO : find a way to return a dynamically allocated pointer. */
	struct AlloyHostData *host_data = &global_host_data;

	host_data->page_size = 2 * 1024 * 1024;

	host_data->heap_size = host_data->page_size * 2;
	host_data->heap_data = (void *) 0x00;

	b_mem_allocate(&host_data->heap_data, host_data->heap_size / host_data->page_size);
	if (host_data->heap_data == ((void *) 0x00))
		return ALLOY_NULL;

	int err = alloy_heap_init(&host_data->heap, host_data->heap_data, host_data->heap_size);
	if (err != 0)
	{
		b_mem_release(&host_data->heap_data, host_data->heap_size / host_data->page_size);
		return ALLOY_NULL;
	}

	return host_data;
}

static void host_done(struct AlloyHostData *host_data)
{
	if (host_data->heap_data != ALLOY_NULL)
	{
		b_mem_release(host_data->heap_data, host_data->heap_size / host_data->page_size);
		host_data->heap_data = ALLOY_NULL;
		host_data->heap_size = 0;
	}
}

static struct AlloyDir *host_opendir(struct AlloyHostData *host_data,
                                     const char *path)
{
	struct AlloyDir *dir = alloy_heap_malloc(&host_data->heap, sizeof(struct AlloyDir));
	if (dir == ALLOY_NULL)
		return ALLOY_NULL;

	(void) path;

	return dir;
}

static struct AlloyDirEnt *host_readdir(struct AlloyHostData *host_data,
                                        struct AlloyDir *dir)
{
	(void) host_data;
	(void) dir;
	return ALLOY_NULL;
}

static void host_closedir(struct AlloyHostData *host_data,
                          struct AlloyDir *dir)
{
	if (dir != ALLOY_NULL)
	{
		alloy_heap_free(&host_data->heap, dir);
	}
}

static int host_get_info(struct AlloyHostData *host_data,
                         struct AlloyHostInfo *host_info)
{
	host_info->page_size = host_data->page_size;
	return 0;
}

static void *host_alloc_pages(struct AlloyHostData *host_data,
                              alloy_size page_count)
{
	(void) host_data;

	void *addr = ALLOY_NULL;

	b_mem_allocate(&addr, page_count);

	return addr;
}

static void host_free_pages(struct AlloyHostData *host_data,
                            alloy_size page_count,
                            void *pages_addr)
{
	(void) host_data;
	b_mem_release(&pages_addr, page_count);
}

const struct AlloyHost alloy_host = {
	host_init,
	host_done,
	host_opendir,
	host_readdir,
	host_closedir,
	host_get_info,
	host_alloc_pages,
	host_free_pages
};

char b_input_key(void) {
	char chr;
	asm volatile ("call *0x00100010" : "=a" (chr));
	return chr;
}

unsigned long b_mem_allocate(void **mem, unsigned long nbr) {
	unsigned long mem_addr;
	unsigned long tlong;
	asm volatile ("call *0x00100030" : "=a"(mem_addr), "=c"(tlong) : "c"(nbr));
	(*mem) = (void *)(mem_addr);
	return tlong;
}

unsigned long b_mem_release(void **mem, unsigned long nbr) {
	unsigned long tlong;
	unsigned long mem_addr;
	mem_addr = (unsigned long)(*mem);
	asm volatile ("call *0x00100038" : "=c"(tlong) : "a"(mem_addr), "c"(nbr));
	return tlong;
}

unsigned long b_disk_read(void *mem, unsigned long start, unsigned long num, unsigned long disknum) {
	unsigned long tlong;
	asm volatile ("call *0x00100050" : "=c"(tlong) : "a"(start), "c"(num), "d"(disknum), "D"(mem));
	return tlong;
}

unsigned long b_disk_write(const void *mem, unsigned long start, unsigned long num, unsigned long disknum) {
	unsigned long tlong = 0;
	asm volatile ("call *0x00100058" : "=c"(tlong) : "a"(start), "c"(num), "d"(disknum), "S"(mem));
	return tlong;
}

