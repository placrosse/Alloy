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

/** @defgroup color-api Color API
 * Used to define colors for the terminal.
 * */

/** Represents a single RGB color.
 * @ingroup color-api
 * */

struct AlloyColor
{
	/** The red value of the color.
	 * A value of 255 means maximum red. */
	alloy_uint8 red;
	/** The green value of the color.
	 * A value of 255 means maximum green. */
	alloy_uint8 green;
	/** The blue value of the color.
	 * A value of 255 means maximum blue. */
	alloy_uint8 blue;
};

/** Parses a color string in the
 * form of either '#rrggbb', 'rrggbb',
 * or another well known color.
 * @param color The color structure to
 * initialize based on the string.
 * @param str The string containing the
 * color specification.
 * @returns Zero on success, an error code
 * on failure.
 * @ingroup color-api
 * */

int alloy_color_parse(struct AlloyColor *color,
                      const char *str);

/** The color red.
 * @ingroup color-api
 * */

extern const struct AlloyColor alloy_red;

/** The color blue.
 * @ingroup color-api
 * */

extern const struct AlloyColor alloy_blue;

/** The color green.
 * @ingroup color-api
 * */

extern const struct AlloyColor alloy_green;

/** The color white.
 * @ingroup color-api
 * */

extern const struct AlloyColor alloy_white;

/** The color black.
 * @ingroup color-api
 * */

extern const struct AlloyColor alloy_black;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ALLOY_COLOR_H */
