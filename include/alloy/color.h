/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_COLOR_H
#define ALLOY_COLOR_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyColor
{
	alloy_uint8 red;
	alloy_uint8 green;
	alloy_uint8 blue;
};

extern const struct AlloyColor alloy_red;

extern const struct AlloyColor alloy_blue;

extern const struct AlloyColor alloy_green;

extern const struct AlloyColor alloy_white;

extern const struct AlloyColor alloy_black;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ALLOY_COLOR_H */
