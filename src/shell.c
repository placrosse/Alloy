#include <alloy/shell.h>

#include <alloy/term.h>
#include <alloy/types.h>

void alloy_shell_init(struct AlloyShell *shell)
{
	shell->term = ALLOY_NULL;
	shell->term_data = ALLOY_NULL;
	alloy_input_init(&shell->input);
}

void alloy_shell_done(struct AlloyShell *shell)
{
	if (shell->term != ALLOY_NULL)
	{
		alloy_term_done(shell->term, shell->term_data);
		shell->term = ALLOY_NULL;
		shell->term_data = ALLOY_NULL;
	}
}
