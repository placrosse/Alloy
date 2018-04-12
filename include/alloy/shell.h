#ifndef ALLOY_SHELL_H
#define ALLOY_SHELL_H

#include <alloy/input.h>
#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyColor;
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

int alloy_shell_clear(struct AlloyShell *shell);

int alloy_shell_clear_line(struct AlloyShell *shell);

int alloy_shell_get_char(struct AlloyShell *shell,
                         alloy_utf8 *c);

int alloy_shell_get_line(struct AlloyShell *shell);

int alloy_shell_set_foreground(struct AlloyShell *shell,
                               const struct AlloyColor *color);

void alloy_shell_set_term(struct AlloyShell *shell,
                          const struct AlloyTerm *term);

int alloy_shell_write(struct AlloyShell *shell,
                      const char *str,
                      alloy_size str_size);

int alloy_shell_write_asciiz(struct AlloyShell *shell,
                             const char *str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SHELL_H */
