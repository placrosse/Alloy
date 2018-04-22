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

	err = alloy_path_parse_z(&path, "/this//is\\a/file.txt");
	assert(err == 0);

	assert(path.name_count == 4);
	assert(alloy_strcmp(path.names[0], "this") == 0);
	assert(alloy_strcmp(path.names[1], "is") == 0);
	assert(alloy_strcmp(path.names[2], "a") == 0);
	assert(alloy_strcmp(path.names[3], "file.txt") == 0);

	/* Cleanup and exit. */

	alloy_path_done(&path);

	return 0;
}
