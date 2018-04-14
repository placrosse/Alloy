/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/host.h>

#include <alloy/errno.h>

struct AlloyHostData *alloy_host_init(const struct AlloyHost *host)
{
	if (host->init == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return host->init();
}

void alloy_host_done(const struct AlloyHost *host,
                     struct AlloyHostData *host_data)
{
	if (host->done != ALLOY_NULL)
		host->done(host_data);
}

int alloy_host_get_info(const struct AlloyHost *host,
                        struct AlloyHostData *host_data,
                        struct AlloyHostInfo *host_info)
{
	if (host->get_info == ALLOY_NULL)
		return ALLOY_ENOSYS;

	return host->get_info(host_data, host_info);
}

void *alloy_host_alloc_pages(const struct AlloyHost *host,
                             struct AlloyHostData *host_data,
                             alloy_size page_count)
{
	if (host->alloc_pages == ALLOY_NULL)
		return ALLOY_NULL;

	return host->alloc_pages(host_data, page_count);
}
