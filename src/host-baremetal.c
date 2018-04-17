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

const bmfs_uint64 fs_sector = 8;

struct AlloyDisk
{
	/** Contains the disk callbacks. */
	struct BMFSDisk base;
	/** A buffer to put temporary sector data into. */
	unsigned char sector[4096];
	/** The position of next disk operation. */
	bmfs_uint64 pos;
};

static int alloy_disk_read(void *disk_ptr,
                           void *buf,
                           bmfs_uint64 buf_size,
                           bmfs_uint64 *read_size)
{
	struct AlloyDisk *disk = (struct AlloyDisk *) disk_ptr;

	if (buf_size > 4096)
		buf_size = 4096;

	unsigned long int sectors_read = b_disk_read(disk->sector, fs_sector + (disk->pos / 4096), 1, 0);

	bmfs_uint64 bytes_read = sectors_read * 4096;

	if (bytes_read > buf_size)
		bytes_read = buf_size;

	if (bytes_read > (4096 - (disk->pos % 4096)))
		bytes_read = 4096 - (disk->pos % 4096);

	if (read_size != BMFS_NULL)
		*read_size = bytes_read;

	alloy_memcpy(buf, &disk->sector[disk->pos % 4096], bytes_read);

	disk->pos += bytes_read;

	return 0;
}

static int alloy_disk_write(void *disk_ptr,
                            const void *buf,
                            bmfs_uint64 buf_size,
                            bmfs_uint64 *write_size)
{
	struct AlloyDisk *disk = (struct AlloyDisk *) disk_ptr;

	unsigned long int sectors_read = b_disk_read(disk->sector, fs_sector + (disk->pos / 4096), 1, 0);
	if (sectors_read == 0)
		return BMFS_EIO;

	if (buf_size > 4096)
		buf_size = 4096;

	alloy_memcpy(&disk->sector[disk->pos % 4096], buf, buf_size);

	unsigned long int sectors_written = b_disk_write(disk->sector, fs_sector + (disk->pos / 4096), 1, 0);

	bmfs_uint64 bytes_written = sectors_written * 4096;
	if (bytes_written > buf_size)
		bytes_written = buf_size;

	if (write_size != BMFS_NULL)
		*write_size = bytes_written;

	disk->pos += bytes_written;

	return 0;
}

static int alloy_disk_seek(void *disk_ptr,
                           bmfs_uint64 offset,
                           int whence)
{
	struct AlloyDisk *disk = (struct AlloyDisk *) disk_ptr;

	if (whence == BMFS_SEEK_SET)
		disk->pos = offset;
	else
		/* TODO : support BMFS_SEEK_END */
		return BMFS_EINVAL;

	return 0;
}

static int alloy_disk_tell(void *disk_ptr,
                           bmfs_uint64 *offset)
{
	struct AlloyDisk *disk = (struct AlloyDisk *) disk_ptr;

	if (offset != BMFS_NULL)
		*offset = disk->pos;

	return 0;
}

static void alloy_disk_init(struct AlloyDisk *disk)
{
	bmfs_disk_init(&disk->base);
	disk->base.disk = disk;
	disk->base.read = alloy_disk_read;
	disk->base.write = alloy_disk_write;
	disk->base.seek = alloy_disk_seek;
	disk->base.tell = alloy_disk_tell;
	disk->pos = 0;
}

int bmfs_get_current_time(bmfs_uint64 *time)
{
	if (time != BMFS_NULL)
		*time = 0;

	return BMFS_ENOSYS;
}

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

