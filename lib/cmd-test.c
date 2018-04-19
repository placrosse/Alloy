/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/cmd.h>

#include <alloy/heap.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	unsigned char buf[512];

	struct AlloyHeap heap;
	alloy_heap_init(&heap, buf, sizeof(buf));

	struct AlloyCmd cmd;
	alloy_cmd_init(&cmd);
	alloy_cmd_set_heap(&cmd, &heap);

	int err = alloy_cmd_parse(&cmd, " dir\t\'./path1\' ./path2");
	assert(err == 0);
	assert(cmd.id == ALLOY_CMD_DIR);
	assert(cmd.argc == 3);
	assert(strcmp(cmd.argv[0], "dir") == 0);
	assert(strcmp(cmd.argv[1], "./path1") == 0);
	assert(strcmp(cmd.argv[2], "./path2") == 0);
	assert(cmd.argv[3] == ALLOY_NULL);

	return EXIT_SUCCESS;
}
