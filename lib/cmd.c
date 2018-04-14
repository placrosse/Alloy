/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/cmd.h>

#include <alloy/errno.h>
#include <alloy/heap.h>

static int push_arg(struct AlloyCmd *cmd,
                    const char *arg,
                    alloy_size arglen)
{
	if (cmd->heap == ALLOY_NULL)
		return ALLOY_EFAULT;

	alloy_size argc = cmd->argc + 1;

	char **argv = cmd->argv;

	argv = alloy_heap_realloc(cmd->heap, argv, (argc + 1) * sizeof(char *));
	if (argv == ALLOY_NULL)
		return ALLOY_ENOMEM;

	cmd->argv = argv;

	char *new_arg = alloy_heap_malloc(cmd->heap, arglen + 1);
	if (new_arg == ALLOY_NULL)
		return ALLOY_ENOMEM;

	for (alloy_size i = 0; i < arglen; i++)
		new_arg[i] = arg[i];

	new_arg[arglen] = 0;

	argv[argc - 1] = new_arg;

	argv[argc] = ALLOY_NULL;

	cmd->argc++;

	return 0;
}

static void free_args(struct AlloyCmd *cmd)
{
	if (cmd->heap == ALLOY_NULL)
		return;

	for (alloy_size i = 0; i < cmd->argc; i++)
		alloy_heap_free(cmd->heap, &cmd->argv[i]);

	alloy_heap_free(cmd->heap, cmd->argv);

	cmd->argv = ALLOY_NULL;
	cmd->argc = 0;
}

enum AlloyCmdID alloy_cmd_id_parse_z(const char *cmd)
{
	alloy_size i = 0;

	while (cmd[i] != 0)
		i++;

	return alloy_cmd_id_parse(cmd, i);
}

enum AlloyCmdID alloy_cmd_id_parse(const char *cmd, alloy_size cmd_len)
{
	if ((cmd_len == 3)
	 && (cmd[0] == 'c')
	 && (cmd[1] == 'l')
	 && (cmd[2] == 's'))
		return ALLOY_CMD_CLEAR;

	if ((cmd_len == 5)
	 && (cmd[0] == 'c')
	 && (cmd[1] == 'l')
	 && (cmd[2] == 'e')
	 && (cmd[3] == 'a')
	 && (cmd[4] == 'r'))
		return ALLOY_CMD_CLEAR;

	if ((cmd_len == 3)
	 && (cmd[0] == 'd')
	 && (cmd[1] == 'i')
	 && (cmd[2] == 'r'))
		return ALLOY_CMD_DIR;

	if ((cmd_len == 4)
	 && (cmd[0] == 'e')
	 && (cmd[1] == 'x')
	 && (cmd[2] == 'i')
	 && (cmd[3] == 't'))
		return ALLOY_CMD_EXIT;

	if ((cmd_len == 4)
	 && (cmd[0] == 'h')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'l')
	 && (cmd[3] == 'p'))
		return ALLOY_CMD_HELP;

	if ((cmd_len == 2)
	 && (cmd[0] == 'l')
	 && (cmd[1] == 's'))
		return ALLOY_CMD_DIR;

	if ((cmd_len == 3)
	 && (cmd[0] == 'v')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'r'))
		return ALLOY_CMD_VERSION;

	if ((cmd_len == 7)
	 && (cmd[0] == 'v')
	 && (cmd[1] == 'e')
	 && (cmd[2] == 'r')
	 && (cmd[3] == 's')
	 && (cmd[4] == 'i')
	 && (cmd[5] == 'o')
	 && (cmd[6] == 'n'))
		return ALLOY_CMD_VERSION;

	return ALLOY_CMD_UNKNOWN;
}

void alloy_cmd_init(struct AlloyCmd *cmd)
{
	cmd->id = ALLOY_CMD_UNKNOWN;
	cmd->argv = ALLOY_NULL;
	cmd->argc = 0;
}

void alloy_cmd_done(struct AlloyCmd *cmd)
{
	free_args(cmd);
}

void alloy_cmd_set_heap(struct AlloyCmd *cmd,
                        struct AlloyHeap *heap)
{
	if (heap != ALLOY_NULL)
		cmd->heap = heap;
}

int alloy_cmd_parse(struct AlloyCmd *cmd, const char *line)
{
	if (cmd->heap == ALLOY_NULL)
		return ALLOY_EFAULT;

	free_args(cmd);

	alloy_size argpos = 0;
	alloy_size arglen = 0;
	alloy_size i = 0;

	while (line[i] != 0)
	{
		if ((line[i] == ' ')
		 || (line[i] == '\t')
		 || (line[i] == '\n'))
		{
			if (arglen == 0)
			{
				i++;
				continue;
			}

			int err = push_arg(cmd, &line[argpos], arglen);
			if (err != 0)
				return err;

			arglen = 0;
		}
		else
		{
			if (arglen == 0)
				argpos = i;

			arglen++;
		}

		i++;
	}

	if (arglen > 0)
	{
		int err = push_arg(cmd, &line[argpos], arglen);
		if (err != 0)
			return err;
	}

	if (cmd->argc > 0)
		cmd->id = alloy_cmd_id_parse_z(cmd->argv[0]);

	return 0;
}
