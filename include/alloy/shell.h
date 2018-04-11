#ifndef ALLOY_SHELL_H
#define ALLOY_SHELL_H

#include <alloy/input.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyTerm;
struct AlloyTermData;

/** The structure containing all the
 * callbacks necessary for the alloy program.
 * */

struct AlloyShell
{
	/** Contains the callbacks for the terminal interface. */
	const struct AlloyTerm *term;
	/** Contains the terminal implementation data. */
	struct AlloyTermData *term_data;
	/** The input text. */
	struct AlloyInput input;
};

void alloy_shell_init(struct AlloyShell *shell);

void alloy_shell_done(struct AlloyShell *shell);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SHELL_H */
