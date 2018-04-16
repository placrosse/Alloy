/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/string.h>

#include <alloy/heap.h>

int alloy_strcmp(const char *a, const char *b)
{
	for (;;)
	{
		if (*a > *b)
			return 1;
		else if (*a < *b)
			return -1;
		else if (*a == 0)
			break;

		a++;
		b++;
	}

	return 0;
}

char *alloy_strdup(struct AlloyHeap *heap, const char *str)
{
	alloy_size str_len = alloy_strlen(str);

	char *str_2 = alloy_heap_malloc(heap, str_len + 1);
	if (str_2 == ALLOY_NULL)
		return ALLOY_NULL;

	for (alloy_size i = 0; i < str_len; i++)
		str_2[i] = str[i];

	str_2[str_len] = 0;

	return str_2;
}

alloy_size alloy_strlen(const char *str)
{
	alloy_size i = 0;

	while (str[i] != 0)
		i++;

	return i;
}
