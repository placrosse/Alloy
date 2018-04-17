/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_TYPES_H
#define ALLOY_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup types Types
 * Used throughout the library to ensure
 * consistency of data types.
 * */

/** Defines the largest type available on the system.
 * Ideal for representing indices in memory.
 * @ingroup types
 * */

typedef unsigned long long int alloy_size;

/** Defines the largest signed type available on the system.
 * Ideal for returning read counts or write counts, because
 * it can assume a negative value to indicate an error.
 * @ingroup types
 * */

typedef signed long long int alloy_ssize;

/** A 64-bit unsigned integer.
 * @ingroup types
 * */

typedef unsigned long long int alloy_uint64;

/** A 32-bit unsigned integer.
 * @ingroup types
 * */

typedef unsigned int alloy_uint32;

/** A 16-bit unsigned integer.
 * @ingroup types
 * */

typedef unsigned short int alloy_uint16;

/** An 8-bit unsigned integer.
 * @ingroup types
 * */

typedef unsigned char alloy_uint8;

/** A code unit for UTF-32 encoding.
 * @ingroup types
 * */

typedef alloy_uint32 alloy_utf32;

/** A code unit for UTF-8 encoding.
 * @ingroup types
 * */

typedef char alloy_utf8;

/** Used to specify a boolean type.
 * @ingroup types
 * */

typedef unsigned char alloy_bool;

/** Used to indicate an invalid memory address.
 * @ingroup types
 * */

#define ALLOY_NULL ((void *) 0x00)

/** Indicates a boolean true value.
 * @ingroup types
 * */

#define ALLOY_TRUE 1

/** Indicates a boolean false value.
 * @ingroup types
 * */

#define ALLOY_FALSE 0

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_TYPES_H */
