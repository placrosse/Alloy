/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_SHELL_H
#define ALLOY_SHELL_H

#include <alloy/scheme.h>
#include <alloy/heap.h>
#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHost;
struct AlloyHostData;
struct AlloyInput;
struct AlloyPath;
struct AlloyTerm;
struct AlloyTermData;

/** The structure containing all the
 * callbacks necessary for the alloy program.
 * */

struct AlloyShell
{
	/** Contains the callbacks for the host interface. */
	const struct AlloyHost *host;
	/** Contains the implementation data for the host. */
	struct AlloyHostData *host_data;
	/** Used to manage memory allocated by the shell
	 * and sub structures of the shell. */
	struct AlloyHeap heap;
	/** Contains the callbacks for the terminal interface. */
	const struct AlloyTerm *term;
	/** Contains the terminal implementation data. */
	struct AlloyTermData *term_data;
	/** The input text. */
	struct AlloyInput *input;
	/** Set to @ref ALLOY_TRUE if the shell should exit. */
	alloy_bool quit_flag;
	/** Set to @ref ALLOY_TRUE if the heap was initialized. */
	alloy_bool heap_ready;
	/** Set to @ref ALLOY_TRUE if the shell interacts with a user.
	 * This affects the behavior of the shell under certain conditions. */
	alloy_bool interactive;
	/** The color scheme for the terminal interface. */
	struct AlloyScheme scheme;
	/** The variable table for the shell.  */
	struct AlloyVarTable *var_table;
	/** The path to the current directory. */
	struct AlloyPath *path;
};

void alloy_shell_init(struct AlloyShell *shell);

void alloy_shell_done(struct AlloyShell *shell);

int alloy_shell_chdir(struct AlloyShell *shell,
                      const char *path);

int alloy_shell_run(struct AlloyShell *shell);

int alloy_shell_run_once(struct AlloyShell *shell);

void alloy_shell_set_host(struct AlloyShell *shell,
                          const struct AlloyHost *host);

void alloy_shell_set_term(struct AlloyShell *shell,
                          const struct AlloyTerm *term);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SHELL_H */
