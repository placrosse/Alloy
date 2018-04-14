/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/heap.h>

#include <assert.h>
#include <stdlib.h>

int main(void) {

	unsigned char buf[512];

	struct AlloyHeap heap;

	int err = alloy_heap_init(&heap, buf, sizeof(buf));
	assert(err == 0);

	/* Ensure that allocations do not overlap. */

	unsigned char *addr1 = alloy_heap_malloc(&heap, 24);
	assert(addr1 != ALLOY_NULL);

	unsigned char *addr2 = alloy_heap_malloc(&heap, 32);
	assert(addr2 != ALLOY_NULL);
	assert(addr2 >= (addr1 + 24));

	unsigned char *addr3 = alloy_heap_malloc(&heap, 48);
	assert(addr3 != ALLOY_NULL);
	assert(addr3 >= (addr2 + 32));

	return EXIT_SUCCESS;
}
