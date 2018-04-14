/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_SCHEME_H
#define ALLOY_SCHEME_H

#include <alloy/color.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup scheme-api Scheme API
 * Deals with the color scheme for the
 * terminal interface.
 * */

/** Describes the color scheme for
 * the terminal interface.
 * @ingroup scheme-api
 * */

struct AlloyScheme
{
	/** The color of a builtin command. */
	struct AlloyColor cmd_builtin;
	/** The color of an external command. */
	struct AlloyColor cmd_external;
	/** The default background color. */
	struct AlloyColor normal_background;
	/** The default foreground color. */
	struct AlloyColor normal_foreground;
	/** The color of a string literal. */
	struct AlloyColor string_literal;
	/** The color of a numerical. */
	struct AlloyColor numerical;
	/** The color of a comment. */
	struct AlloyColor comment;
	/** The color of an error. */
	struct AlloyColor error;
};

/** Initializes a scheme to the default.
 * @param scheme The scheme structure to initialize.
 * @ingroup scheme-api
 * */

void alloy_scheme_init(struct AlloyScheme *scheme);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SCHEME_H */
