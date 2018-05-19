/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "bmfs-disk.h"

#include <alloy/string.h>
#include <baremetal/syscalls.h>
#include <bmfs/errno.h>

const bmfs_uint64 fs_sector = 8;

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

void alloy_disk_init(struct AlloyDisk *disk)
{
	bmfs_disk_init(&disk->base);
	disk->base.DiskPtr = disk;
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
