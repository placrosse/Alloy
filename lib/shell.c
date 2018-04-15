#include <alloy/shell.h>

#include <alloy/cmd.h>
#include <alloy/errno.h>
#include <alloy/keys.h>
#include <alloy/host.h>
#include <alloy/input.h>
#include <alloy/scanner.h>
#include <alloy/term.h>
#include <alloy/types.h>
#include <alloy/version.h>

static int shell_write(struct AlloyShell *shell,
                       const char *str,
                       alloy_size str_size)
{
	int err = alloy_term_write(shell->term, shell->term_data, str, str_size);
	if (err != 0)
		return err;

	return 0;
}

static int shell_write_asciiz(struct AlloyShell *shell,
                              const char *str)
{
	int err = alloy_term_write_asciiz(shell->term, shell->term_data, str);
	if (err != 0)
		return err;

	return 0;
}

static int init_host(struct AlloyShell *shell)
{
	if (shell->host == ALLOY_NULL)
		return ALLOY_ENOSYS;

	if (shell->host_data != ALLOY_NULL)
		return 0;

	shell->host_data = alloy_host_init(shell->host);
	if (shell->host_data == ALLOY_NULL)
		return ALLOY_ENOMEM;

	return 0;
}

static int init_heap(struct AlloyShell *shell)
{
	/* Check if heap was already initialized. */
	if (shell->heap_ready)
		return 0;

	/* Initialize host if needed. */
	int err = init_host(shell);
	if (err != 0)
		return err;

	struct AlloyHostInfo host_info;

	err = alloy_host_get_info(shell->host, shell->host_data, &host_info);
	if (err != 0)
		return err;

	alloy_size page_size = host_info.page_size;

	/* Give the heap 2MiB */
	alloy_size heap_size = 2 * 1024 * 1024;

	alloy_size heap_page_count = (heap_size + (page_size - 1)) / page_size;

	void *heap = alloy_host_alloc_pages(shell->host, shell->host_data, heap_page_count);
	if (heap == ALLOY_NULL)
		return ALLOY_ENOMEM;

	err = alloy_heap_init(&shell->heap, heap, heap_page_count * page_size);
	if (err != 0)
		return err;

	shell->heap_ready = ALLOY_TRUE;

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

static int init_input(struct AlloyShell *shell)
{
	if (shell->input != ALLOY_NULL)
		return 0;

	int err = init_heap(shell);
	if (err != 0)
		return err;

	shell->input = alloy_heap_malloc(&shell->heap, sizeof(struct AlloyInput));
	if (shell->input == ALLOY_NULL)
		return ALLOY_ENOMEM;

	alloy_input_init(shell->input);

	alloy_input_set_heap(shell->input, &shell->heap);

	return 0;
}

static int shell_set_foreground(struct AlloyShell *shell,
                               const struct AlloyColor *color)
{
	int err = alloy_term_set_foreground(shell->term, shell->term_data, color);
	if (err != 0)
		return err;

	return 0;
}

static int shell_set_background(struct AlloyShell *shell,
                               const struct AlloyColor *color)
{
	int err = alloy_term_set_background(shell->term, shell->term_data, color);
	if (err != 0)
		return err;

	return 0;
}

static int shell_prompt(struct AlloyShell *shell)
{
	alloy_term_clear_line(shell->term, shell->term_data);

	shell_write_asciiz(shell, "> ");

	struct AlloyScanner scanner;

	alloy_scanner_init(&scanner);

	alloy_scanner_set_buf(&scanner, shell->input->buf, shell->input->buf_len);

	while (!alloy_scanner_eof(&scanner))
	{
		struct AlloyToken *token = alloy_scanner_next(&scanner);
		if (token == ALLOY_NULL)
			break;

		if (token->id == ALLOY_TOKEN_IDENTIFIER)
		{
			enum AlloyCmdID cmd_id = alloy_cmd_id_parse(token->buf, token->buf_len);
			if (cmd_id == ALLOY_CMD_UNKNOWN)
			{
				shell_write(shell, token->buf, token->buf_len);
			}
			else
			{
				shell_set_foreground(shell, &shell->scheme.cmd_builtin);
				shell_write(shell, token->buf, token->buf_len);
				shell_set_foreground(shell, &shell->scheme.normal_foreground);
			}
		}
		else if (token->id == ALLOY_TOKEN_NUMERICAL)
		{
			shell_set_foreground(shell, &shell->scheme.numerical);
			shell_write(shell, token->buf, token->buf_len);
			shell_set_foreground(shell, &shell->scheme.normal_foreground);
		}
		else if (token->id == ALLOY_TOKEN_COMMENT)
		{
			shell_set_foreground(shell, &shell->scheme.comment);
			shell_write(shell, token->buf, token->buf_len);
			shell_set_foreground(shell, &shell->scheme.normal_foreground);
		}
		else if (token->id == ALLOY_TOKEN_STRING)
		{
			shell_set_foreground(shell, &shell->scheme.string_literal);
			shell_write(shell, token->buf, token->buf_len);
			shell_set_foreground(shell, &shell->scheme.normal_foreground);
		}
		else if (token->id == ALLOY_TOKEN_ERROR)
		{
			shell_set_foreground(shell, &shell->scheme.error);
			shell_write(shell, token->buf, token->buf_len);
			shell_set_foreground(shell, &shell->scheme.normal_foreground);
		}
		else
		{
			shell_write(shell, token->buf, token->buf_len);
		}
	}

	return 0;
}

static int shell_get_char(struct AlloyShell *shell,
                          alloy_utf8 *c)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = alloy_term_get_char(shell->term, shell->term_data, c);
	if (err != 0)
		return err;

	if (*c == ALLOY_KEY_QUIT)
		shell->quit_flag = ALLOY_TRUE;

	return 0;
}

static int shell_get_line(struct AlloyShell *shell)
{
	for (;;)
	{
		int err = shell_prompt(shell);
		if (err != 0)
			return err;

		alloy_utf8 c = 0;

		err = shell_get_char(shell, &c);
		if ((err != 0) || (c == ALLOY_KEY_QUIT))
			break;

		if (c == ALLOY_KEY_RETURN)
		{
			shell_write(shell, "\n", 1);
			break;
		}

		if (c == ALLOY_KEY_BACKSPACE)
		{
			alloy_input_backspace(shell->input);
			continue;
		}

		alloy_input_insert(shell->input, c);
	}

	return 0;
}

static struct AlloyDir *shell_opendir(struct AlloyShell *shell,
                                      const char *path)
{
	return alloy_host_opendir(shell->host, shell->host_data, path);
}

static struct AlloyDirEnt *shell_readdir(struct AlloyShell *shell,
                                      struct AlloyDir *dir)
{
	return alloy_host_readdir(shell->host, shell->host_data, dir);
}

static void shell_closedir(struct AlloyShell *shell,
                                       struct AlloyDir *dir)
{
	alloy_host_closedir(shell->host, shell->host_data, dir);
}

static int cmd_version(struct AlloyShell *shell)
{
	shell_write_asciiz(shell, ALLOY_VERSION_STRING);
	shell_write_asciiz(shell, "\n");
	return 0;
}

static int cmd_help_print_cmd(struct AlloyShell *shell,
                              const char *name,
                              const char *desc)
{
	shell_write_asciiz(shell, "  ");

	shell_set_foreground(shell, &shell->scheme.cmd_builtin);
	shell_write_asciiz(shell, name);
	shell_set_foreground(shell, &shell->scheme.normal_foreground);

	shell_write_asciiz(shell, " : ");
	shell_write_asciiz(shell, desc);
	shell_write_asciiz(shell, "\n");

	return 0;
}

static int cmd_help(struct AlloyShell *shell)
{
	shell_write_asciiz(shell, "Commands:\n");
	cmd_help_print_cmd(shell, "help   ", "Get help with a command.");
	cmd_help_print_cmd(shell, "exit   ", "Exit the shell.");
	cmd_help_print_cmd(shell, "version", "Print this version of Alloy.");
	cmd_help_print_cmd(shell, "dir    ", "List directory contents.");
	cmd_help_print_cmd(shell, "clear  ", "Clear the screen.");
	return 0;
}

static int cmd_clear(struct AlloyShell *shell)
{
	int err = alloy_term_clear(shell->term, shell->term_data);
	if (err != 0)
		return err;

	return 0;
}

static int cmd_dir(struct AlloyShell *shell,
                   struct AlloyCmd *cmd)
{
	const char *path = "./";

	if (cmd->argc > 1)
		path = cmd->argv[1];

	alloy_bool list_all = ALLOY_FALSE;

	struct AlloyDir *dir = shell_opendir(shell, path);

	if (dir == ALLOY_NULL)
	{
		shell_write_asciiz(shell, "Failed to open ");

		shell_set_foreground(shell, &shell->scheme.string_literal);
		shell_write_asciiz(shell, "'");
		shell_write_asciiz(shell, path);
		shell_write_asciiz(shell, "'");
		shell_set_foreground(shell, &shell->scheme.normal_foreground);

		shell_write_asciiz(shell, "\n");

		return 0;
	}

	for (;;)
	{
		struct AlloyDirEnt *ent = shell_readdir(shell, dir);
		if (ent == ALLOY_NULL)
			break;

		if ((ent->name[0] == '.') && !list_all)
			continue;

		if (ent->type == ALLOY_DIR_ENT_FILE)
			shell_write_asciiz(shell, "  file : ");
		else
			shell_write_asciiz(shell, "  dir  : ");

		shell_set_foreground(shell, &shell->scheme.string_literal);
		shell_write_asciiz(shell, "\"");
		shell_write_asciiz(shell, ent->name);
		shell_write_asciiz(shell, "\"");
		shell_set_foreground(shell, &shell->scheme.normal_foreground);
		shell_write_asciiz(shell, "\n");
	}

	shell_closedir(shell, dir);

	return 0;
}

static int cmd_unknown(struct AlloyShell *shell,
                       struct AlloyCmd *cmd)
{
	/* TODO : check the file system. */

	if (cmd->argc == 0)
	{
		/* No command was passed.
		 * This is okay. */
		return 0;
	}

	shell_write_asciiz(shell, "Unknown command: ");

	shell_set_foreground(shell, &shell->scheme.string_literal);
	shell_write_asciiz(shell, "'");
	shell_write_asciiz(shell, cmd->argv[0]);
	shell_write_asciiz(shell, "'");
	shell_set_foreground(shell, &shell->scheme.normal_foreground);

	shell_write_asciiz(shell, ".\n");

	return 0;
}

static int shell_run_cmd(struct AlloyShell *shell)
{
	struct AlloyCmd cmd;

	alloy_cmd_init(&cmd);

	alloy_cmd_set_heap(&cmd, &shell->heap);

	int err = alloy_cmd_parse(&cmd, shell->input->buf);
	if (err != 0)
	{
		alloy_cmd_done(&cmd);
		alloy_input_clear(shell->input);
		return err;
	}

	alloy_input_clear(shell->input);

	err = 0;

	switch (cmd.id)
	{
	case ALLOY_CMD_CLEAR:
		err = cmd_clear(shell);
		break;
	case ALLOY_CMD_DIR:
		err = cmd_dir(shell, &cmd);
		break;
	case ALLOY_CMD_EXIT:
		shell->quit_flag = ALLOY_TRUE;
		break;
	case ALLOY_CMD_VERSION:
		err = cmd_version(shell);
		break;
	case ALLOY_CMD_HELP:
		err = cmd_help(shell);
		break;
	case ALLOY_CMD_UNKNOWN:
		err = cmd_unknown(shell, &cmd);
		break;
	}

	alloy_cmd_done(&cmd);

	return err;
}

void alloy_shell_init(struct AlloyShell *shell)
{
	shell->heap_ready = ALLOY_FALSE;
	shell->host = ALLOY_NULL;
	shell->host_data = ALLOY_NULL;
	shell->term = ALLOY_NULL;
	shell->term_data = ALLOY_NULL;
	shell->quit_flag = ALLOY_FALSE;
	shell->input = ALLOY_NULL;
	alloy_scheme_init(&shell->scheme);
}

void alloy_shell_done(struct AlloyShell *shell)
{
	if (shell->input != ALLOY_NULL)
	{
		alloy_input_done(shell->input);
		alloy_heap_free(&shell->heap, shell->input);
		shell->input = ALLOY_NULL;
	}

	if (shell->term != ALLOY_NULL)
	{
		alloy_term_done(shell->term, shell->term_data);
		shell->term = ALLOY_NULL;
		shell->term_data = ALLOY_NULL;
	}

	if (shell->host != ALLOY_NULL)
	{
		alloy_host_done(shell->host, shell->host_data);
		shell->host = ALLOY_NULL;
		shell->host_data = ALLOY_NULL;
	}
}

int alloy_shell_run(struct AlloyShell *shell)
{
	while (!shell->quit_flag)
	{
		int err = alloy_shell_run_once(shell);
		if (err != 0)
			return err;
	}

	return 0;
}

int alloy_shell_run_once(struct AlloyShell *shell)
{
	int err = init_term(shell);
	if (err != 0)
		return err;

	err = init_input(shell);
	if (err != 0)
	{
		shell_write_asciiz(shell, "Failed to initialize input.\n");
		return err;
	}

	err = shell_get_line(shell);
	if (err != 0)
	{
		shell_write_asciiz(shell, "Failed to read input line.\n");
		return err;
	}

	shell_set_foreground(shell, &shell->scheme.normal_foreground);
	shell_set_background(shell, &shell->scheme.normal_background);

	err = shell_run_cmd(shell);
	if (err != 0)
	{
		shell_write_asciiz(shell, "Failed to run command.\n");
		return err;
	}

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

	err = init_input(shell);
	if (err != 0)
		return err;

	alloy_input_clear(shell->input);

	return 0;
}

void alloy_shell_set_host(struct AlloyShell *shell,
                          const struct AlloyHost *host)
{
	if (shell->host != ALLOY_NULL)
		alloy_host_done(shell->host, shell->host_data);

	shell->host = host;
	shell->host_data = ALLOY_NULL;
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