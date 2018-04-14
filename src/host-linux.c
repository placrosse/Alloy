/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "host.h"

#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <string.h>

struct AlloyHostData
{
	alloy_size page_size;
};

struct AlloyDir
{
	struct {
		DIR *dir;
		struct dirent *ent;
	} host;
	struct AlloyDirEnt current_ent;
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

static struct AlloyDir *host_opendir(struct AlloyHostData *host_data,
                                     const char *path)
{
	(void) host_data;

	struct AlloyDir *dir = malloc(sizeof(*dir));
	if (dir == NULL)
		return ALLOY_NULL;

	dir->host.dir = opendir(path);
	if (dir->host.dir == NULL)
	{
		free(dir);
		return ALLOY_NULL;
	}

	return dir;
}

static struct AlloyDirEnt *host_readdir(struct AlloyHostData *host_data,
                                        struct AlloyDir *dir)
{
	(void) host_data;

	dir->host.ent = readdir(dir->host.dir);
	if (dir->host.ent == NULL)
		return ALLOY_NULL;

	strcpy(dir->current_ent.name, dir->host.ent->d_name);

	/* TODO support other file types. */

	if (dir->host.ent->d_type == DT_DIR)
		dir->current_ent.type = ALLOY_DIR_ENT_DIR;
	else
		dir->current_ent.type = ALLOY_DIR_ENT_FILE;

	return &dir->current_ent;
}

static void host_closedir(struct AlloyHostData *host_data,
                          struct AlloyDir *dir)
{
	(void) host_data;

	if (dir != ALLOY_NULL)
	{
		if (dir->host.dir != NULL)
			closedir(dir->host.dir);

		free(dir);
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
	host_opendir,
	host_readdir,
	host_closedir,
	host_get_info,
	host_alloc_pages,
	host_free_pages
};
