/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_HEAP_H
#define ALLOY_HEAP_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup heap-api Heap API
 * Used to implement a basic heap
 * for dynamic memory allocation.
 * */

/** A heap allocation entri.
 * @ingroup heap-api
 * */

struct AlloyHeapEnt
{
	/** The address of the allocation. */
	unsigned char *addr;
	/** The number of bytes used by the
	 * allocation. */
	alloy_size size;
};

/** Contains the fields that are
 * used to implement the heap.
 * @ingroup heap-api
 * */

struct AlloyHeap
{
	/** The address of the start of the heap. */
	unsigned char *addr;
	/** The number of bytes available for the heap. */
	alloy_size size;
	/** The allocation entries on the heap. */
	struct AlloyHeapEnt *entries;
	/** The number of heap allocation entries. */
	alloy_size entry_count;
};

/** Initializes the heap structure.
 * @param heap The heap structure to initialize.
 * @param addr The address of the start of the heap.
 * @param size The number of bytes available for the
 * heap to grow.
 * @returns Zero on success, an error code on failure.
 * @ingroup heap-api
 * */

int alloy_heap_init(struct AlloyHeap *heap,
                    void *addr,
                    alloy_size size);

/** Allocates data on the heap.
 * @param heap An initialized heap structure.
 * @param size The size of the allocation.
 * @returns The address of the allocation on success.
 * On failure, @ref ALLOY_NULL is returned.
 * @ingroup heap-api
 * */

void *alloy_heap_malloc(struct AlloyHeap *heap,
                        alloy_size size);

/** Releases memory on the heap.
 * @param heap An initialized heap structure.
 * @param addr The address to free on the heap.
 * @ingroup heap-api
 * */

void alloy_heap_free(struct AlloyHeap *heap,
                     void *addr);

/** Resizes an existing heap allocation.
 *  @param heap An initialized heap structure.
 *  @param size The number of bytes the new
 *  allocation should be able to fit.
 *  @param addr The address of the previous
 *  allocation.
 *  @returns On success, the new address of
 *  the allocation. On failure, @ref ALLOY_NULL.
 *  @ingroup heap-api
 * */

void *alloy_heap_realloc(struct AlloyHeap *heap,
                         void *addr,
                         alloy_size size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_HEAP_H */
