/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/shell.h>

#include "term.h"
#include "host.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

int main(void)
{
	/* setup the shell */

	struct AlloyShell shell;

	alloy_shell_init(&shell);

	alloy_shell_set_host(&shell, &alloy_host);

	alloy_shell_set_term(&shell, &alloy_term);

	int err = alloy_shell_run(&shell);
	if (err != 0)
	{
		alloy_shell_done(&shell);
		return EXIT_FAILURE;
	}

	alloy_shell_done(&shell);

	return EXIT_SUCCESS;
}
