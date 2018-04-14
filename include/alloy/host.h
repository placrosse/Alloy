/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_HOST_H
#define ALLOY_HOST_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct AlloyDir;
struct AlloyFile;
struct AlloyHostData;

/** @defgroup host-api Host API
 * Used by Alloy to interface with the
 * host machine in an abstract manner.
 * */

/** Describes the host machine.
 * @ingroup host-api
 * */

struct AlloyHostInfo
{
	/** The number of bytes in one memory page. */
	alloy_size page_size;
};

/** The type of directory entry.
 * @ingroup host-api
 * */

enum AlloyDirEntType
{
	/** The entry is a file. */
	ALLOY_DIR_ENT_FILE,
	/** The entry is a directory. */
	ALLOY_DIR_ENT_DIR
};

/** Contains information about a directory
 * entry on the host system.
 * @ingroup host-api
 * */

struct AlloyDirEnt
{
	/** The name of the directory entry. */
	char name[256];
	/** Indicates what type of entry this is. */
	enum AlloyDirEntType type;
};

/** Used to implement the interface that
 * Alloy requires from the host machine.
 * @ingroup host-api
 * */

struct AlloyHost
{
	/** Initializes the host implementation data. */
	struct AlloyHostData *(*init)(void);
	/** Releases memory from the host implementation data. */
	void (*done)(struct AlloyHostData *host_data);
	/** Opens a directory on the host system. */
	struct AlloyDir *(*opendir)(struct AlloyHostData *host_data,
	                            const char *path);
	/** Reads a directory entry from a directory. */
	struct AlloyDirEnt *(*readdir)(struct AlloyHostData *host_data,
	                               struct AlloyDir *dir);
	/** Closes a directory on the host system. */
	void (*closedir)(struct AlloyHostData *host_data,
	                 struct AlloyDir *dir);
	/** Gets information about the host. */
	int (*get_info)(struct AlloyHostData *host_data,
	                struct AlloyHostInfo *host_info);
	/** Allocates a specified number of pages. */
	void *(*alloc_pages)(struct AlloyHostData *host_data,
	                     alloy_size page_count);
	/** Releases a certain number of pages. */
	void (*free_pages)(struct AlloyHostData *host_data,
	                   alloy_size page_count,
	                   void *pages_addr);
};

/** Initializes host implementation data.
 * @param host An initialized host structure.
 * @ingroup host-api
 * */

struct AlloyHostData *alloy_host_init(const struct AlloyHost *host);

/** Releases memory allocated by the host implementation data.
 * @param host An initialized host structure.
 * @param host_data The host implementation data.
 * @ingroup host-api
 * */

void alloy_host_done(const struct AlloyHost *host,
                     struct AlloyHostData *host_data);

/** Opens a directory for reading.
 * @param host An initialized host structure.
 * @param host_data Host implementation data.
 * @param path The path to the directory.
 * @returns On success, a pointer to a directory
 * structure is returned. On failure, @ref ALLOY_NULL
 * is returned.
 * @ingroup host-api
 * */

struct AlloyDir *alloy_host_opendir(const struct AlloyHost *host,
                                    struct AlloyHostData *host_data,
                                    const char *path);

/** Reads a directory entry from a directory.
 * @param host An initialized host structure.
 * @param host_data Host implementation data.
 * @param dir A directory structure opened
 * with @ref alloy_host_opendir.
 * @returns On success, a pointer to a directory
 * entry structure is returned. On failure,
 * @ref ALLOY_NULL is returned.
 * @ingroup host-api
 * */

struct AlloyDirEnt *alloy_host_readdir(const struct AlloyHost *host,
                                       struct AlloyHostData *host_data,
                                       struct AlloyDir *dir);

/** Closes a directory.
 * @param host An initialized host structure.
 * @param host_data Host implementation data.
 * @param dir A directory structure opened
 * with @ref alloy_host_opendir.
 * @ingroup host-api
 */

void alloy_host_closedir(const struct AlloyHost *host,
                         struct AlloyHostData *host_data,
                         struct AlloyDir *dir);

/** Get information about the host.
 * @param host An initialized host structure.
 * @param host_data Host implementation data.
 * @param host_info The structure that will end
 * up containing the host information.
 * @returns Zero on success, an error code on failure.
 * @ingroup host-api
 * */

int alloy_host_get_info(const struct AlloyHost *host,
                        struct AlloyHostData *host_data,
                        struct AlloyHostInfo *host_info);

/** Allocates a series of pages.
 * @param host An initialized host structure.
 * @param host_data Host implementation data.
 * @param page_count The number of pages to allocate.
 * @returns On success, the address of the pages.
 * On failure, @ref ALLOY_NULL is returned.
 * @ingroup host-api
 * */

void *alloy_host_alloc_pages(const struct AlloyHost *host,
                             struct AlloyHostData *host_data,
                             alloy_size page_count);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_HOST_H */
