/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/cmd.h>

enum AlloyCmdID alloy_cmd_id_parse(const char *cmd)
{
	if ((cmd[0] == 'c')
	 && (cmd[1] == 'l')
	 && (cmd[2] == 's')
	 && (cmd[3] == 0))
		return ALLOY_CMD_CLEAR;

	if ((cmd[0] == 'c')
	 && (cmd[1] == 'l')
	 && (cmd[2] == 'e')
	 && (cmd[3] == 'a')
	 && (cmd[4] == 'r')
	 && (cmd[5] == 0))
		return ALLOY_CMD_CLEAR;

	if ((cmd[0] == 'd')
	 && (cmd[1] == 'i')
	 && (cmd[2] == 'r')
	 && (cmd[3] == 0))
		return ALLOY_CMD_DIR;

	if ((cmd[0] == 'e')
	 && (cmd[1] == 'x')
	 && (cmd[2] == 'i')
	 && (cmd[3] == 't')
	 && (cmd[4] == 0))
		return ALLOY_CMD_EXIT;

	if ((cmd[0] == 'h')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'l')
	 && (cmd[3] == 'p')
	 && (cmd[4] == 0))
		return ALLOY_CMD_HELP;

	if ((cmd[0] == 'l')
	 && (cmd[1] == 's')
	 && (cmd[2] == 0))
		return ALLOY_CMD_DIR;

	if ((cmd[0] == 'v')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'r')
	 && (cmd[3] == 0))
		return ALLOY_CMD_VERSION;

	if ((cmd[0] == 'v')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'r')
	 && (cmd[3] == 's')
	 && (cmd[4] == 'i')
	 && (cmd[5] == 'o')
	 && (cmd[6] == 'n')
	 && (cmd[7] == 0))
		return ALLOY_CMD_VERSION;

	return ALLOY_CMD_UNKNOWN;
}

void alloy_cmd_init(struct AlloyCmd *cmd)
{
	cmd->id = ALLOY_CMD_UNKNOWN;
}

int alloy_cmd_parse(struct AlloyCmd *cmd, const char *line)
{
	(void) cmd;
	(void) line;
	return 0;
}
