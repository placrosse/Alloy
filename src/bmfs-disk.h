/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_PRIVATE_BMFS_DISK_H
#define ALLOY_PRIVATE_BMFS_DISK_H

#include <bmfs/disk.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyDisk
{
	/** Contains the disk callbacks. */
	struct BMFSDisk base;
	/** A buffer to put temporary sector data into. */
	unsigned char sector[4096];
	/** The position of next disk operation. */
	bmfs_uint64 pos;
};

void alloy_disk_init(struct AlloyDisk *disk);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_PRIVATE_BMFS_DISK_H */
