/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_THEME_H
#define ALLOY_THEME_H

#include <alloy/color.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup theme-api Theme API
 * Deals with the color theme for the
 * terminal interface.
 * */

/** Describes the theme for the
 * terminal interface.
 * @ingroup theme-api
 * */

struct AlloyTheme
{
	/** The color of a builtin command. */
	struct AlloyColor cmd_builtin;
	/** The color of an external command. */
	struct AlloyColor cmd_external;
};

/** Initializes a theme to the default.
 * @param theme The theme structure to initialize.
 * @ingroup theme-api
 * */

void alloy_theme_init(struct AlloyTheme *theme);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_THEME_H */
