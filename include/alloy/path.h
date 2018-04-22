/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_PATH_H
#define ALLOY_PATH_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** @defgroup path-api Path API
 * Creates, modifies, and resolves paths to strings.
 * */

/** A file system path.
 * This structure is generally used
 * when the path needs to be manipulated
 * in some way. This includes the resovling
 * of relative paths.
 * @ingroup path-api
 * */

struct AlloyPath
{
	/** A pointer to the heap that allocates
	 * the path names. */
	struct AlloyHeap *heap;
	/** The array of names that make up
	 * the path. The last name is a null
	 * pointer. */
	char **names;
	/** The number of names in the path.
	 * The root directory is considered
	 * to have zero names. The null terminating
	 * path name is not included in this number. */
	alloy_size name_count;
};

/** Initializes a path structure for use.
 * @param path The path structure to initialize.
 * @param heap The heap used to allocate the
 * path names on. This must not be @ref ALLOY_NULL.
 * @ingroup path-api
 * */

void alloy_path_init(struct AlloyPath *path,
                     struct AlloyHeap *heap);

/** Releases memory allocated by the path.
 * @param path An initialized path structure.
 * @ingroup path-api
 * */

void alloy_path_done(struct AlloyPath *path);

/** Appends to the end of the path structure.
 * The path that is appended is not normalized.
 * @param path An initialized path structure.
 * @param subpath_str The path string to append.
 * @returns Zero on success, an error code on failure.
 * @ingroup path-api
 * */

int alloy_path_append(struct AlloyPath *path,
                      const char *subpath_str);

/** Resolves all references to '..' and '.'.
 * @param path An initialized path structure.
 * @returns Zero on success, an error code on failure.
 * @ingroup path-api
 * */

int alloy_path_normalize(struct AlloyPath *path);

/** Parses a path from a string.
 * @param path An initialized path structure.
 * @param str The string to parse the path on.
 * @param str_size The number of bytes in the string.
 * @returns Zero on success, an error code on failure.
 * @ingroup path-api
 * */

int alloy_path_parse(struct AlloyPath *path,
                     const char *str,
                     alloy_size str_size);

/** Parses path from a null-terminated string.
 * @param path An initialized path structure.
 * @param str The string to parse the path on.
 * @returns Zero on success, an error code on failure.
 * @ingroup path-api
 * */

int alloy_path_parse_z(struct AlloyPath *path,
                       const char *str);

/** Converts the path structure to a string.
 * @param path An initialized path structure.
 * @param heap The heap structure to allocate
 * the string on.
 * @returns A null-terminated string representation
 * of the path structure.
 * @ingroup path-api
 * */

char *alloy_path_to_string(struct AlloyPath *path,
                           struct AlloyHeap *heap);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_PATH_H */
