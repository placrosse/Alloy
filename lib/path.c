/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/path.h>

#include <alloy/errno.h>
#include <alloy/heap.h>
#include <alloy/string.h>

static alloy_bool is_separator(char c)
{
	if ((c == '/')
	 || (c == '\\'))
		return ALLOY_TRUE;
	else
		return ALLOY_FALSE;
}

static int push_name(struct AlloyPath *path,
                     const char *name,
                     alloy_size name_size)
{
	if (path->heap == ALLOY_NULL)
		return ALLOY_EFAULT;

	if (name_size == 0)
		return 0;

	char *tmp = alloy_heap_malloc(path->heap, name_size + 1);
	if (tmp == ALLOY_NULL)
		return ALLOY_ENOMEM;

	alloy_memcpy(tmp, name, name_size);

	tmp[name_size] = 0;

	char **names = path->names;

	alloy_size name_count = path->name_count + 1;

	names = alloy_heap_realloc(path->heap, names, (name_count + 1) * sizeof(char *));
	if (names == ALLOY_NULL)
	{
		alloy_heap_free(path->heap, tmp);
		return ALLOY_ENOMEM;
	}

	names[name_count - 1] = tmp;
	names[name_count] = ALLOY_NULL;

	path->names = names;
	path->name_count = name_count;

	return 0;
}

void alloy_path_init(struct AlloyPath *path,
                     struct AlloyHeap *heap)
{
	path->names = ALLOY_NULL;
	path->name_count = 0;
	path->heap = heap;
}

void alloy_path_done(struct AlloyPath *path)
{
	if (path->heap == ALLOY_NULL)
		return;

	for (alloy_size i = 0; i < path->name_count; i++)
		alloy_heap_free(path->heap, path->names[i]);

	alloy_heap_free(path->heap, path->names);

	path->names = ALLOY_NULL;
	path->name_count = 0;
}

int alloy_path_append(struct AlloyPath *path,
                      const char *subpath_str)
{
	return alloy_path_parse_z(path, subpath_str);
}

int alloy_path_normalize(struct AlloyPath *path)
{
	alloy_size i = 0;

	while (i < path->name_count)
	{
		char *name = path->names[i];
		if (alloy_strcmp(name, ".") == 0)
		{
			alloy_heap_free(path->heap, name);
			for (alloy_size j = i + 1; j < path->name_count; j++)
			{
				path->names[j - 1] = path->names[j];
			}

			path->name_count--;
		}
		else if (alloy_strcmp(name, "..") == 0)
		{
			alloy_heap_free(path->heap, name);
			if (i == 0)
			{
				path->name_count--;
				continue;
			}

			i--;

			alloy_heap_free(path->heap, path->names[i]);

			for (alloy_size j = i + 2; j < path->name_count; j++)
			{
				path->names[j - 2] = path->names[j];
			}

			path->name_count -= 2;
		}
		else
		{
			i++;
		}
	}

	return 0;
}

int alloy_path_parse(struct AlloyPath *path,
                     const char *str,
                     alloy_size str_size)
{
	const char *name = ALLOY_NULL;

	alloy_size name_size = 0;

	for (alloy_size i = 0; i < str_size; i++)
	{
		if (is_separator(str[i]))
		{
			if (name_size == 0)
				continue;

			int err = push_name(path, name, name_size);
			if (err != 0)
				return err;

			name = ALLOY_NULL;
			name_size = 0;
		}
		else
		{
			if (name_size == 0)
				name = &str[i];

			name_size++;
		}
	}

	if (name_size > 0)
		return push_name(path, name, name_size);

	return 0;
}

int alloy_path_parse_z(struct AlloyPath *path,
                       const char *str)
{
	return alloy_path_parse(path, str, alloy_strlen(str));
}

char *alloy_path_to_string(struct AlloyPath *path,
                           struct AlloyHeap *heap)
{
	char *str = ALLOY_NULL;

	alloy_size str_size = 0;

	for (alloy_size i = 0; i < path->name_count; i++)
	{
		alloy_size name_size = alloy_strlen(path->names[i]);

		char *tmp = alloy_heap_realloc(heap, str, str_size + name_size + 2);
		if (tmp == ALLOY_NULL)
		{
			alloy_heap_free(heap, str);
			return ALLOY_NULL;
		}

		str = tmp;

		str[str_size] = '/';

		for (alloy_size j = 0; j < name_size; j++)
			str[str_size + j + 1] = path->names[i][j];

		str_size += name_size + 1;

		str[str_size] = 0;
	}

	return str;
}
