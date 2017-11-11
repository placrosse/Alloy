#ifndef ALLOY_SHELL_H
#define ALLOY_SHELL_H

#include <alloy/input.h>
#include <alloy/term.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** The structure containing all the
 * callbacks necessary for the alloy program.
 * */

struct AlloyShell
{
	/** The terminal interface. */
	struct AlloyTerm term;
	/** The input text. */
	struct AlloyInput input;
};

void alloy_shell_init(struct AlloyShell *shell);

void alloy_shell_done(struct AlloyShell *shell);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SHELL_H */
