/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/heap.h>

#include <alloy/errno.h>

static void *find_region(struct AlloyHeap *heap,
                         alloy_size size)
{
	unsigned char *addr = heap->addr;

	for (alloy_size i = 0; i < heap->entry_count; i++)
	{
		unsigned char *start = addr;
		unsigned char *stop = addr + size;

		struct AlloyHeapEnt *ent = &heap->entries[i];

		unsigned char *ent_start = ent->addr;
		unsigned char *ent_stop = ent->addr + ent->size;

		/* Check if entries overlap */
		if (((start <= ent_start) && (stop > ent_start))
		 || ((start <= ent_stop) && (stop > ent_stop)))
		{
			/* Skip the candidate address
			 * passed this entry. */
			addr = ent_stop;
		}
	}

	if ((addr + size) > (heap->addr + heap->size))
		return ALLOY_NULL;

	return addr;
}

static struct AlloyHeapEnt *make_ent(struct AlloyHeap *heap)
{
	alloy_size new_size = sizeof(struct AlloyHeapEnt) * (heap->entry_count + 1);

	struct AlloyHeapEnt *entries = heap->entries;

	entries = alloy_heap_realloc(heap, new_size, entries);
	if (entries == ALLOY_NULL)
		return ALLOY_NULL;

	heap->entries = entries;
	heap->entry_count++;

	heap->entries[heap->entry_count - 1].addr = ALLOY_NULL;
	heap->entries[heap->entry_count - 1].size = 0;

	return &heap->entries[heap->entry_count - 1];
}

static struct AlloyHeapEnt *find_ent(struct AlloyHeap *heap,
                                     void *addr)
{
	for (alloy_size i = 0; i < heap->entry_count; i++)
	{
		struct AlloyHeapEnt *ent = &heap->entries[i];
		if (ent->addr == addr)
			return ent;
	}

	return ALLOY_NULL;
}

static void sort_heap(struct AlloyHeap *heap)
{
	for (;;)
	{
		alloy_bool sorted = ALLOY_FALSE;

		for (alloy_size i = 0; (i + 1) < heap->entry_count; i++)
		{
			struct AlloyHeapEnt *a = &heap->entries[i];
			struct AlloyHeapEnt *b = &heap->entries[i + 1];
			if (a->addr > b->addr)
			{
				struct AlloyHeapEnt tmp;
				tmp.addr = a->addr;
				tmp.size = a->size;
				a->addr = b->addr;
				a->size = b->size;
				b->addr = tmp.addr;
				b->size = tmp.size;
				sorted = ALLOY_TRUE;
			}
		}

		if (!sorted)
			break;
	}
}

int alloy_heap_init(struct AlloyHeap *heap,
                    void *addr,
                    alloy_size size)
{
	if (size < sizeof(struct AlloyHeapEnt))
		return ALLOY_ENOMEM;

	heap->addr = (unsigned char *) addr;
	heap->size = size;

	heap->entry_count = 1;

	heap->entries = (struct AlloyHeapEnt *) addr;

	heap->entries[0].addr = addr;
	heap->entries[0].size = sizeof(struct AlloyHeapEnt);

	return 0;
}

void *alloy_heap_malloc(struct AlloyHeap *heap,
                        alloy_size size)
{
	struct AlloyHeapEnt *new_ent = make_ent(heap);
	if (new_ent == ALLOY_NULL)
		return ALLOY_NULL;

	new_ent->addr = find_region(heap, size);
	if (new_ent->addr == ALLOY_NULL)
		return ALLOY_NULL;

	new_ent->size = size;

	sort_heap(heap);

	return new_ent->addr;
}

void *alloy_heap_realloc(struct AlloyHeap *heap,
                         alloy_size size,
                         void *addr)
{
	if (addr == ALLOY_NULL)
		return alloy_heap_malloc(heap, size);

	struct AlloyHeapEnt *ent = find_ent(heap, addr);
	if (ent == ALLOY_NULL)
		return ALLOY_NULL;

	unsigned char *old_addr = ent->addr;

	unsigned char *new_addr = find_region(heap, size);
	if (new_addr == ALLOY_NULL)
		return ALLOY_NULL;

	ent->addr = new_addr;
	ent->size = size;

	for (alloy_size i = 0; (i < size) && (i < ent->size); i++)
		new_addr[i] = old_addr[i];

	sort_heap(heap);

	return new_addr;
}

void alloy_heap_free(struct AlloyHeap *heap,
                     void *addr)
{
	struct AlloyHeapEnt *ent = find_ent(heap, addr);
	if (ent == ALLOY_NULL)
		return;

	/* Set the address so that it is
	 * the largest possible address.
	 * That will cause the sort mechanism
	 * to put it at the end of the list.
	 * */

	ent->addr = (void *) ~0;
	ent->size = 0;

	sort_heap(heap);

	heap->entry_count--;
}
