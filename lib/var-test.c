/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/var.h>
#include <alloy/heap.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	unsigned char buf[512];

	struct AlloyHeap heap;
	alloy_heap_init(&heap, buf, sizeof(buf));

	struct AlloyVarTable var_table;
	alloy_var_table_init(&var_table, &heap);

	int err = alloy_var_table_define(&var_table, "PATH", "/usr/bin");
	assert(err == 0);

	struct AlloyVar *var = alloy_var_table_find(&var_table, "PATH");
	assert(var != ALLOY_NULL);
	assert(var->name != ALLOY_NULL);
	assert(var->value != ALLOY_NULL);
	assert(strcmp(var->name, "PATH") == 0);
	assert(strcmp(var->value, "/usr/bin") == 0);

	const char *value = alloy_var_table_get(&var_table, "PATH");
	assert(value != ALLOY_NULL);
	assert(strcmp(value, "/usr/bin") == 0);

	var = alloy_var_table_find(&var_table, "DOES_NOT_EXIST");
	assert(var == ALLOY_NULL);

	err = alloy_var_table_define(&var_table, "PATH", "/bin");
	assert(err == 0);

	value = alloy_var_table_get(&var_table, "PATH");
	assert(value != ALLOY_NULL);
	assert(strcmp(value, "/bin") == 0);

	return EXIT_SUCCESS;
}
