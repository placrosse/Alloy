#ifndef ALLOY_TYPES_H
#define ALLOY_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Defines the largest type available on
 * the system. Ideal for representing indices
 * in memory.
 * @ingroup types
 * */

typedef unsigned long long int alloy_size;

/** Used to indicate an invalid memory address.
 * @ingroup types
 * */

#define ALLOY_NULL ((void *) 0x00)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_TYPES_H */
