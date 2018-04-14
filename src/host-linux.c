/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "host.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

struct AlloyHostData {
	alloy_size page_size;
};

static struct AlloyHostData *host_init(void)
{
	struct AlloyHostData *host_data = malloc(sizeof(*host_data));
	if (host_data == NULL)
		return ALLOY_NULL;

	long page_size = sysconf(_SC_PAGE_SIZE);
	if (page_size == -1L)
	{
		free(host_data);
		return ALLOY_NULL;
	}

	host_data->page_size = (alloy_size) page_size;

	return host_data;
}

static void host_done(struct AlloyHostData *host_data)
{
	free(host_data);
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
	alloy_size size = page_count * host_data->page_size;

	void *addr = mmap(0, size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1 /* fd */, 0 /* offset */);

	if (addr == MAP_FAILED)
		return ALLOY_NULL;

	return addr;
}

static void host_free_pages(struct AlloyHostData *host_data,
                            alloy_size page_count,
                            void *pages_addr)
{
	munmap(pages_addr, page_count * host_data->page_size);
}

const struct AlloyHost alloy_host = {
	host_init,
	host_done,
	host_get_info,
	host_alloc_pages,
	host_free_pages
};
