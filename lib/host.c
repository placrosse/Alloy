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

struct AlloyFile *alloy_host_open(const struct AlloyHost *host,
                                  struct AlloyHostData *host_data,
                                  const char *path,
                                  enum AlloyFileMode mode)
{
	if (host->open == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return host->open(host_data, path, mode);
}

void alloy_host_close(const struct AlloyHost *host,
                      struct AlloyHostData *host_data,
                      struct AlloyFile *file)
{
	if (host->close != ALLOY_NULL)
		return host->close(host_data, file);
}

alloy_ssize alloy_host_read(const struct AlloyHost *host,
                            struct AlloyHostData *host_data,
                            struct AlloyFile *file,
                            void *buf,
                            alloy_size buf_size)
{
	if (host->read == ALLOY_NULL)
		return -1;
	else
		return host->read(host_data, file, buf, buf_size);
}

struct AlloyDir *alloy_host_opendir(const struct AlloyHost *host,
                                    struct AlloyHostData *host_data,
                                    const char *path)
{
	if (host->opendir == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return host->opendir(host_data, path);
}

struct AlloyDirEnt *alloy_host_readdir(const struct AlloyHost *host,
                                       struct AlloyHostData *host_data,
                                       struct AlloyDir *dir)
{
	if (host->readdir == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return host->readdir(host_data, dir);
}

void alloy_host_closedir(const struct AlloyHost *host,
                         struct AlloyHostData *host_data,
                         struct AlloyDir *dir)
{
	if (host->closedir != ALLOY_NULL)
		host->closedir(host_data, dir);
}

