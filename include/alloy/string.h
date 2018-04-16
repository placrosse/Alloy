/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_STRING_H
#define ALLOY_STRING_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** Compares two null-terminated strings.
 * @param a The primary string to compare.
 * @param b The secondary string to compare.
 * @returns If @p has a character greater than
 * @p b, then one is returned. If @p a has a
 * character less than @p b, then negative one
 * is returned. Otherwise if the strings are the
 * same, then zero is returned.
 * */

int alloy_strcmp(const char *a, const char *b);

/** Uses the heap to duplicate a string.
 * @param heap The heap to allocate the new string on.
 * @param str The string to duplicate. This must be null-terminated.
 * @returns On success, a pointer to the new string is returned.
 * On failure, @ref ALLOY_NULL is returned.
 * */

char *alloy_strdup(struct AlloyHeap *heap, const char *str);

/** Calculates the length of a string.
 * @param str The string to calculate the length of.
 * @returns The number of bytes occupied by the string.
 * */

alloy_size alloy_strlen(const char *str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_STRING_H */
