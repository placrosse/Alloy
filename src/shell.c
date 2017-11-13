#include <alloy/shell.h>

void alloy_shell_init(struct AlloyShell *shell)
{
	alloy_term_init(&shell->term);
	alloy_input_init(&shell->input);
}

void alloy_shell_done(struct AlloyShell *shell)
{
	alloy_term_done(&shell->term);
}
