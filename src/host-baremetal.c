/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "host.h"

#include <baremetal/syscalls.h>

struct AlloyHostData
{
	alloy_size page_size;
};

struct AlloyDir
{

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

struct AlloyHostData global_host_data;

static struct AlloyHostData *host_init(void)
{
	struct AlloyHostData *host_data = &global_host_data;

	host_data->page_size = 2 * 1024 * 1024;

	return host_data;
}

static void host_done(struct AlloyHostData *host_data)
{
	(void) host_data;
}

static struct AlloyDir *host_opendir(struct AlloyHostData *host_data,
                                     const char *path)
{
	(void) host_data;
	(void) path;
	return ALLOY_NULL;
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
	(void) host_data;
	(void) dir;
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
