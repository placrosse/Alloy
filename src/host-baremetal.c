/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "host.h"

#include <alloy/heap.h>
#include <alloy/string.h>

#include <baremetal/syscalls.h>

#include <bmfs/bmfs.h>

#include "bmfs-host.h"
#include "bmfs-disk.h"

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
	/** A disk structure used to interface
	 * with the file system. */
	struct AlloyDisk disk;
	/** The file system. */
	struct BMFS bmfs;
	/** Whether or not the file system was found. */
	alloy_bool bmfs_found;
};

struct AlloyDir
{
	struct {
		/** The actual directory structure. */
		struct BMFSDir dir;
	} host;
	/** The current entry of the directory. */
	struct AlloyDirEnt ent;
};

struct AlloyFile
{
	struct BMFSFile file;
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

	alloy_disk_init(&host_data->disk);

	bmfs_init(&host_data->bmfs);

	bmfs_set_disk(&host_data->bmfs, &host_data->disk.base);

	bmfs_set_host(&host_data->bmfs, &alloy_bmfs_host);

	if (bmfs_import(&host_data->bmfs) == 0)
		host_data->bmfs_found = ALLOY_TRUE;
	else
		host_data->bmfs_found = ALLOY_FALSE;

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

static struct AlloyFile *host_open(struct AlloyHostData *host_data,
                                   const char *path,
                                   enum AlloyFileMode mode)
{
	if (!host_data->bmfs_found)
		return ALLOY_NULL;

	struct AlloyHeap *heap = &host_data->heap;

	struct AlloyFile *file = alloy_heap_malloc(heap, sizeof(*file));
	if (file == ALLOY_NULL)
		return ALLOY_NULL;

	int err = bmfs_open_file(&host_data->bmfs, &file->file, path);
	if (err != 0)
	{
		alloy_heap_free(heap, file);
		return ALLOY_NULL;
	}

	switch (mode)
	{
	case ALLOY_FILE_MODE_READ:
		bmfs_file_set_mode(&file->file, BMFS_FILE_MODE_READ);
		break;
	case ALLOY_FILE_MODE_WRITE:
		bmfs_file_set_mode(&file->file, BMFS_FILE_MODE_WRITE);
		break;
	case ALLOY_FILE_MODE_RW:
		bmfs_file_set_mode(&file->file, BMFS_FILE_MODE_RW);
		break;
	}

	return file;
}

static void host_close(struct AlloyHostData *host_data,
                       struct AlloyFile *file)
{
	if (file == ALLOY_NULL)
		return;

	bmfs_file_close(&file->file);

	struct AlloyHeap *heap = &host_data->heap;

	alloy_heap_free(heap, file);
}

static alloy_ssize host_read(struct AlloyHostData *host_data,
                             struct AlloyFile *file,
                             void *buf,
                             alloy_size buf_size)
{
	(void) host_data;

	bmfs_uint64 read_size = 0;

	int err = bmfs_file_read(&file->file, buf, buf_size, &read_size);
	if (err != 0)
		return -1;

	return (alloy_ssize) read_size;
}

static struct AlloyDir *host_opendir(struct AlloyHostData *host_data,
                                     const char *path)
{
	if (!host_data->bmfs_found)
		return ALLOY_NULL;

	struct AlloyDir *dir = alloy_heap_malloc(&host_data->heap, sizeof(struct AlloyDir));
	if (dir == ALLOY_NULL)
		return ALLOY_NULL;

	int err = bmfs_open_dir(&host_data->bmfs, &dir->host.dir, path);
	if (err != 0)
	{
		alloy_heap_free(&host_data->heap, dir);
		return ALLOY_NULL;
	}

	return dir;
}

static struct AlloyDirEnt *host_readdir(struct AlloyHostData *host_data,
                                        struct AlloyDir *dir)
{
	struct BMFSEntry *entry = bmfs_dir_next(&dir->host.dir);
	if (entry == BMFS_NULL)
		return ALLOY_NULL;

	if (bmfs_entry_is_directory(entry))
		dir->ent.type = ALLOY_DIR_ENT_DIR;
	else
		dir->ent.type = ALLOY_DIR_ENT_FILE;

	if (sizeof(dir->ent.name) > sizeof(entry->Name))
		alloy_memcpy(dir->ent.name, entry->Name, sizeof(entry->Name));
	else
		alloy_memcpy(dir->ent.name, entry->Name, sizeof(dir->ent.name));

	(void) host_data;

	return &dir->ent;
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
	host_open,
	host_close,
	host_read,
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

