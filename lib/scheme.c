/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/scheme.h>

/* The default scheme is based on this molokai scheme:
 *  - 0x272822
 *  - 0xf92672
 *  - 0x66d9ef
 *  - 0xa6e22e
 *  - 0xfd971f
 * */

void alloy_scheme_init(struct AlloyScheme *scheme)
{
	scheme->cmd_builtin.red = 0xf9;
	scheme->cmd_builtin.green = 0x26;
	scheme->cmd_builtin.blue = 0x72;

	scheme->cmd_external.red = 0xf9;
	scheme->cmd_external.green = 0x26;
	scheme->cmd_external.blue = 0x72;

	scheme->normal_foreground.red = 0xff;
	scheme->normal_foreground.green = 0xff;
	scheme->normal_foreground.blue = 0xff;

	scheme->normal_background.red = 0x27;
	scheme->normal_background.green = 0x28;
	scheme->normal_background.blue = 0x22;

	scheme->string_literal.red = 0xa6;
	scheme->string_literal.green = 0xe2;
	scheme->string_literal.blue = 0x2e;
}
