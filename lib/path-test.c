/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/path.h>

#include <alloy/heap.h>
#include <alloy/string.h>

#include <assert.h>

int main(void)
{
	/* Initialize the heap. */

	unsigned char heap_buf[512];

	struct AlloyHeap heap;

	int err = alloy_heap_init(&heap, heap_buf, sizeof(heap_buf));
	assert(err == 0);

	/* Initialize the path. */

	struct AlloyPath path;

	alloy_path_init(&path, &heap);

	/* Test path parsing. */

	err = alloy_path_parse_z(&path, "/usr//local/.\\../bin/make");
	assert(err == 0);

	assert(path.name_count == 6);
	assert(alloy_strcmp(path.names[0], "usr") == 0);
	assert(alloy_strcmp(path.names[1], "local") == 0);
	assert(alloy_strcmp(path.names[2], ".") == 0);
	assert(alloy_strcmp(path.names[3], "..") == 0);
	assert(alloy_strcmp(path.names[4], "bin") == 0);
	assert(alloy_strcmp(path.names[5], "make") == 0);

	/* Normalize path. */

	err = alloy_path_normalize(&path);
	assert(err == 0);
	assert(path.name_count == 3);
	assert(alloy_strcmp(path.names[0], "usr") == 0);
	assert(alloy_strcmp(path.names[1], "bin") == 0);
	assert(alloy_strcmp(path.names[2], "make") == 0);

	char *path_str = alloy_path_to_string(&path, &heap);

	assert(path_str != ALLOY_NULL);
	assert(alloy_strcmp(path_str, "/usr/bin/make") == 0);

	alloy_heap_free(&heap, path_str);

	/* Cleanup and exit. */

	alloy_path_done(&path);

	return 0;
}
