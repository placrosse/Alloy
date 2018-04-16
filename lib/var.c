/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/var.h>

#include <alloy/errno.h>
#include <alloy/heap.h>
#include <alloy/string.h>

static int new_var(struct AlloyVarTable *var_table,
                   const char *name,
                   const char *value)
{
	alloy_size var_size = sizeof(struct AlloyVar);

	alloy_size var_count = var_table->var_count;
	var_count++;

	struct AlloyVar *var_array = var_table->var_array;

	var_array = alloy_heap_realloc(var_table->heap, var_array, var_count * var_size);
	if (var_array == ALLOY_NULL)
		return ALLOY_ENOMEM;

	var_table->var_array = var_array;

	struct AlloyVar *new_var = &var_array[var_count - 1];

	new_var->name = alloy_strdup(var_table->heap, name);

	if (value != ALLOY_NULL)
		new_var->value = alloy_strdup(var_table->heap, value);
	else
		new_var->value = ALLOY_NULL;

	if (new_var->name == ALLOY_NULL) {
		alloy_heap_free(var_table->heap, new_var->name);
		alloy_heap_free(var_table->heap, new_var->value);
		return ALLOY_ENOMEM;
	}

	var_table->var_count++;

	return 0;
}

void alloy_var_table_init(struct AlloyVarTable *var_table,
                          struct AlloyHeap *heap)
{
	var_table->heap = heap;
	var_table->var_array = ALLOY_NULL;
	var_table->var_count = 0;
}

void alloy_var_table_done(struct AlloyVarTable *var_table)
{
	if (var_table->heap == ALLOY_NULL)
		return;

	for (alloy_size i = 0; i < var_table->var_count; i++)
	{
		alloy_heap_free(var_table->heap, &var_table->var_array[i].name);
		alloy_heap_free(var_table->heap, &var_table->var_array[i].value);
	}

	alloy_heap_free(var_table->heap, var_table->var_array);

	var_table->var_array = ALLOY_NULL;
	var_table->var_count = 0;
}

int alloy_var_table_define(struct AlloyVarTable *var_table,
                           const char *name,
                           const char *value)
{
	if (var_table->heap == ALLOY_NULL)
		return ALLOY_ENOMEM;

	struct AlloyVar *var = alloy_var_table_find(var_table, name);
	if (var != ALLOY_NULL)
	{
		if (value == ALLOY_NULL)
			return 0;

		alloy_heap_free(var_table->heap, var->value);

		var->value = alloy_strdup(var_table->heap, value);
		if (var->value == ALLOY_NULL)
			return ALLOY_ENOMEM;
		else
			return 0;
	}
	else
	{
		int err = new_var(var_table, name, value);
		if (err != 0)
			return err;
	}

	return 0;
}

struct AlloyVar *alloy_var_table_find(const struct AlloyVarTable *var_table,
                                      const char *name)
{
	for (alloy_size i = 0; i < var_table->var_count; i++)
	{
		if (alloy_strcmp(var_table->var_array[i].name, name) == 0)
			return &var_table->var_array[i];
	}

	return ALLOY_NULL;
}

const char *alloy_var_table_get(const struct AlloyVarTable *var_table,
                                const char *name)
{
	struct AlloyVar *var = alloy_var_table_find(var_table, name);
	if (var == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return var->value;
}

const char *alloy_var_table_get_s(const struct AlloyVarTable *var_table,
                                  const char *name,
                                  alloy_size name_size)
{
	for (alloy_size i = 0; i < var_table->var_count; i++)
	{
		const struct AlloyVar *var = &var_table->var_array[i];

		alloy_size j = 0;

		while ((j < name_size) && (var->name[j] != 0))
		{
			if (name[j] != var->name[j])
					break;
			j++;
		}

		if ((j == name_size) && (var->name[j] == 0))
			return var->value;
	}

	return ALLOY_NULL;
}
