/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/heap.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int check_mem(unsigned char *addr,
                     alloy_size size,
                     alloy_uint8 value)
{
	for (alloy_size i = 0; i < size; i++)
	{
		if (addr[i] != value)
			return -1;
	}

	return 0;
}

int main(void) {

	unsigned char buf[512];

	struct AlloyHeap heap;

	int err = alloy_heap_init(&heap, buf, sizeof(buf));
	assert(err == 0);

	/* Ensure that allocations do not overlap. */

	unsigned char *addr1 = alloy_heap_malloc(&heap, 24);
	assert(addr1 != ALLOY_NULL);
	memset(addr1, 0x3f, 24);

	unsigned char *addr2 = alloy_heap_malloc(&heap, 32);
	assert(addr2 != ALLOY_NULL);
	memset(addr2, 0x71, 32);

	unsigned char *addr3 = alloy_heap_malloc(&heap, 48);
	assert(addr3 != ALLOY_NULL);
	memset(addr3, 0x02, 48);

	assert(check_mem(addr1, 24, 0x3f) == 0);
	assert(check_mem(addr2, 32, 0x71) == 0);
	assert(check_mem(addr3, 48, 0x02) == 0);

	/* Test that memory is properly reallocated. */

	/* Assign data so we can check if it was moved
	 * during the reallocation. */
	addr1[0] = '3';
	addr1[1] = '1';
	addr1[2] = '4';
	addr1[3] = '1';
	addr1[22] = '5';
	addr1[23] = '9';

	unsigned char *addr4 = alloy_heap_realloc(&heap, addr1, 64);
	assert(addr4 != ALLOY_NULL);
	/* It's not a requirement that these addresses
	 * are different, but for testing purposes it
	 * should be. The reallocation should be large
	 * enough that the memory has to be moved somewhere
	 * else. That is the only way we can test that the
	 * data is being moved properly. */
	assert(addr4 != addr1);
	assert(addr4[0] == '3');
	assert(addr4[1] == '1');
	assert(addr4[2] == '4');
	assert(addr4[3] == '1');
	assert(addr4[22] == '5');
	assert(addr4[23] == '9');

	/* Check that the other sections of memory are still valid. */
	assert(check_mem(addr2, 32, 0x71) == 0);
	assert(check_mem(addr3, 48, 0x02) == 0);

	/* Free memory. */

	alloy_heap_free(&heap, addr4);

	/* Allocate more memory. */

	unsigned char *addr5 = alloy_heap_malloc(&heap, 64);
	assert(addr5 != ALLOY_NULL);
	memset(addr5, 0x55, 64);

	unsigned char *addr6 = alloy_heap_malloc(&heap, 55);
	assert(addr6 != ALLOY_NULL);
	memset(addr6, 0x31, 55);

	/* Resize existing memory. */

	unsigned char *addr7 = alloy_heap_realloc(&heap, addr2, 28);
	assert(addr7 != ALLOY_NULL);

	unsigned char *addr8 = alloy_heap_realloc(&heap, addr3, 32);
	assert(addr8 != ALLOY_NULL);

	/* Check consistency */

	assert(check_mem(addr5, 64, 0x55) == 0);
	assert(check_mem(addr6, 55, 0x31) == 0);
	assert(check_mem(addr7, 28, 0x71) == 0);
	assert(check_mem(addr8, 32, 0x02) == 0);

	/* Check to ensure that failure works. */

	assert(alloy_heap_malloc(&heap, 512) == ALLOY_NULL);

	return EXIT_SUCCESS;
}
