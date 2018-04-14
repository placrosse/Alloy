/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/theme.h>

void alloy_theme_init(struct AlloyTheme *theme)
{
	theme->cmd_builtin.red = 0xe4;
	theme->cmd_builtin.green = 0xff;
	theme->cmd_builtin.blue = 0xdc;

	theme->cmd_external.red = 0xc0;
	theme->cmd_external.green = 0xe9;
	theme->cmd_external.blue = 0xc8;
}
