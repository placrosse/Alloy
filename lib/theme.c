/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/theme.h>

/* The default theme is based on this molokai scheme:
 *  - 0x272822
 *  - 0xf92672
 *  - 0x66d9ef
 *  - 0xa6e22e
 *  - 0xfd971f
 * */

void alloy_theme_init(struct AlloyTheme *theme)
{
	theme->cmd_builtin.red = 0xf9;
	theme->cmd_builtin.green = 0x26;
	theme->cmd_builtin.blue = 0x72;

	theme->cmd_external.red = 0xf9;
	theme->cmd_external.green = 0x26;
	theme->cmd_external.blue = 0x72;

	theme->normal_foreground.red = 0xff;
	theme->normal_foreground.green = 0xff;
	theme->normal_foreground.blue = 0xff;

	theme->normal_background.red = 0x27;
	theme->normal_background.green = 0x28;
	theme->normal_background.blue = 0x22;

	theme->string_literal.red = 0xa6;
	theme->string_literal.green = 0xe2;
	theme->string_literal.blue = 0x2e;
}
