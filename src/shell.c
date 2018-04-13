#include <alloy/shell.h>

#include <alloy/errno.h>
#include <alloy/keys.h>
#include <alloy/host.h>
#include <alloy/input.h>
#include <alloy/term.h>
#include <alloy/types.h>

static int prompt(struct AlloyShell *shell)
{
	alloy_input_backspace(shell->Input);
	alloy_shell_clear_line(shell);
	alloy_shell_write_asciiz(shell, ">-");
	alloy_shell_write(shell, shell->Input->buf, shell->Input->buf_len);
	return 0;
}

static int init_term(struct AlloyShell *shell)
{
	if (shell->term == ALLOY_NULL)
		return ALLOY_ENOSYS;

	if (shell->term_data != ALLOY_NULL)
		return 0;

	shell->term_data = alloy_term_init(shell->term);
	if (shell->term_data == ALLOY_NULL)
		return ALLOY_ENOMEM;

	return 0;
}

void alloy_shell_init(struct AlloyShell *shell)
{
	shell->term = ALLOY_NULL;
	shell->term_data = ALLOY_NULL;
	shell->QuitFlag = ALLOY_FALSE;
	shell->Input = ALLOY_NULL;
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

int alloy_shell_clear(struct AlloyShell *shell)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_clear(shell->term, shell->term_data);
	if (err != 0)
		return err;

	return 0;
}

int alloy_shell_clear_line(struct AlloyShell *shell)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_clear_line(shell->term, shell->term_data);
	if (err != 0)
		return err;

	return 0;
}

int alloy_shell_get_char(struct AlloyShell *shell,
                         alloy_utf8 *c)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_get_char(shell->term, shell->term_data, c);
	if (err != 0)
		return err;

	if (*c == ALLOY_KEY_QUIT)
		shell->QuitFlag = ALLOY_TRUE;

	return 0;
}

int alloy_shell_get_line(struct AlloyShell *shell)
{
	if (shell->Input == ALLOY_NULL)
		return ALLOY_EFAULT;

	for (;;)
	{
		alloy_utf8 c = 0;

		int err = alloy_shell_get_char(shell, &c);
		if ((err != 0) || (c == ALLOY_KEY_QUIT))
			break;

		if (c == ALLOY_KEY_RETURN)
			break;

		if (c == ALLOY_KEY_BACKSPACE)
		{
			prompt(shell);
			continue;
		}

		alloy_shell_write(shell, &c, 1);

		alloy_input_insert(shell->Input, c);
	}

	return 0;
}

int alloy_shell_set_foreground(struct AlloyShell *shell,
                               const struct AlloyColor *color)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_set_foreground(shell->term, shell->term_data, color);
	if (err != 0)
		return err;

	return 0;
}

void alloy_shell_set_host(struct AlloyShell *shell,
                          const struct AlloyHost *host)
{
	if (shell->host != ALLOY_NULL)
	{
		alloy_host_done(shell->host, shell->host_data);
	}

	shell->host = host;
	shell->host_data = ALLOY_NULL;
}

void alloy_shell_set_input(struct AlloyShell *shell,
                           struct AlloyInput *input)
{
	if (input != ALLOY_NULL)
		shell->Input = input;
}

void alloy_shell_set_term(struct AlloyShell *shell,
                          const struct AlloyTerm *term)
{
	if (shell->term != ALLOY_NULL)
	{
		alloy_term_done(shell->term, shell->term_data);
	}

	shell->term = term;
	shell->term_data = ALLOY_NULL;
}

int alloy_shell_write(struct AlloyShell *shell,
                      const char *str,
                      alloy_size str_size)
{

	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_write(shell->term, shell->term_data, str, str_size);
	if (err != 0)
		return err;

	return 0;
}

int alloy_shell_write_asciiz(struct AlloyShell *shell,
                             const char *str)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_write_asciiz(shell->term, shell->term_data, str);
	if (err != 0)
		return err;

	return 0;
}
