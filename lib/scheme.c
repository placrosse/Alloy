/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/scheme.h>

#include <alloy/string.h>

void alloy_scheme_init(struct AlloyScheme *scheme)
{
	alloy_memcpy(scheme, &alloy_scheme_oceanic_next, sizeof(struct AlloyScheme));
}

const struct AlloyScheme alloy_scheme_molokai = {
	{ 0xf9, 0x26, 0x72 } /* cmd builtin */,
	{ 0xf9, 0x26, 0x72 } /* cmd external */,
	{ 0x27, 0x28, 0x22 } /* normal background */,
	{ 0xff, 0xff, 0xff } /* normal foreground */,
	{ 0xa6, 0xe2, 0x2e } /* string literal */,
	{ 0xfd, 0x97, 0x1f } /* numerical */,
	{ 0x66, 0xd9, 0xef } /* comment */,
	{ 0xba, 0x49, 0x2c } /* error */
};

const struct AlloyScheme alloy_scheme_oceanic_next = {
	{ 0xec, 0x5f, 0x67 } /* cmd builtin */,
	{ 0xec, 0x5f, 0x67 } /* cmd external */,
	{ 0x1b, 0x2b, 0x34 } /* normal background */,
	{ 0xff, 0xff, 0xff } /* normal foreground */,
	{ 0x99, 0xc7, 0x94 } /* string literal */,
	{ 0xf9, 0x91, 0x57 } /* numerical */,
	{ 0x5f, 0xb3, 0xb3 } /* comment */,
	{ 0xff, 0xff, 0xff } /* error */
};
