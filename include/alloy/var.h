/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_VAR_H
#define ALLOY_VAR_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** @defgroup var-api Variables API
 * Used to manipulate and read variable values.
 * */

struct AlloyVar
{
	/** A null terminated string that
	 * contains the name of the variable. */
	char *name;
	/** A null terminated string that
	 * defines the value of the variable. */
	char *value;
};

/** Used to store and retrieve
 * variables.
 * @ingroup var-api
 * */

struct AlloyVarTable
{
	/** The heap used to allocate variables. */
	struct AlloyHeap *heap;
	/** The variable array. */
	struct AlloyVar *var_array;
	/** The number of variables in the table. */
	alloy_size var_count;
};

/** Initializes a variable table.
 * @param var_table The variable table to initialize.
 * @param heap The heap that will be used to allocate
 * the variable entries.
 * @ingroup var-api
 * */

void alloy_var_table_init(struct AlloyVarTable *var_table,
                          struct AlloyHeap *heap);

/** Releases memory allocated by the variable table.
 * @param var_table An initialized variable table.
 * @ingroup var-api
 * */

void alloy_var_table_done(struct AlloyVarTable *var_table);

/** Defines a new variable, or updates the value
 * of an existing one.
 * @param var_table An initialized variable table.
 * @param name The name of the variable to define.
 * @param value The value to give the variable.
 * @returns Zero on success, an error code on failure.
 * @ingroup var-api
 * */

int alloy_var_table_define(struct AlloyVarTable *var_table,
                           const char *name,
                           const char *value);

/** Locates a variable entry.
 * @param var_table An initialized variable table.
 * @param name The name of the variable to find.
 * @returns The pointer to the variable entry, if
 * it exists. If it does not exist, then @ref ALLOY_NULL
 * is returned instead.
 * @ingroupv var-api
 * */

struct AlloyVar *alloy_var_table_find(const struct AlloyVarTable *var_table,
                                      const char *name);

/** Gets the value of an existing variable.
 * @param var_table An initialized variable table.
 * @param name The name of the variable.
 * @returns The value of the variable, if it is found.
 * If it cannot be found, then @ref ALLOY_NULL is
 * returned instead.
 * @ingroup var-api
 * */

const char *alloy_var_table_get(const struct AlloyVarTable *var_table,
                                const char *name);

/** Gets the value of an existing variable.
 * The size of the variable name must be passed
 * to this function. To pass a null-terminated
 * string, use @ref alloy_var_table_get instead.
 * @param var_table An initialized variable table.
 * @param name The name of the variable.
 * This string does not have to be null-terminated.
 * @param name_size The number of characters in
 * the variable name.
 * @returns The value of the variable, if it exists.
 * If it does not exist, then @ref ALLOY_NULL is returned.
 * @ingroup var-api
 * */

const char *alloy_var_table_get_s(const struct AlloyVarTable *var_table,
                                  const char *name,
                                  alloy_size name_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_VAR_H */
