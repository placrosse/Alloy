/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "bmfs-host.h"

const struct BMFSHost alloy_bmfs_host = {
	BMFS_NULL /* Init */,
	BMFS_NULL /* Done */,
	BMFS_NULL /* Malloc */,
	BMFS_NULL /* Free */,
	BMFS_NULL /* Lock */,
	BMFS_NULL /* Unlock */
};
