/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/shell.h>

#include <alloy/app.h>
#include <alloy/cmd.h>
#include <alloy/errno.h>
#include <alloy/keys.h>
#include <alloy/host.h>
#include <alloy/input.h>
#include <alloy/scanner.h>
#include <alloy/string.h>
#include <alloy/term.h>
#include <alloy/types.h>
#include <alloy/var.h>
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

static int shell_write_z(struct AlloyShell *shell,
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

	/* Give the heap 16MiB */
	alloy_size heap_size = 16 * 1024 * 1024;

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

	alloy_term_set_foreground(shell->term, shell->term_data, &shell->scheme.normal_foreground);
	alloy_term_set_background(shell->term, shell->term_data, &shell->scheme.normal_background);

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

static int init_var_table(struct AlloyShell *shell)
{
	if (shell->var_table != ALLOY_NULL)
		return 0;

	int err = init_heap(shell);
	if (err != 0)
		return err;

	shell->var_table = alloy_heap_malloc(&shell->heap, sizeof(struct AlloyVarTable));
	if (shell->var_table == ALLOY_NULL)
		return ALLOY_ENOMEM;

	alloy_var_table_init(shell->var_table, &shell->heap);

	return 0;
}

static char *shell_append(struct AlloyShell *shell,
                          char *begin,
                          const char *end,
                          alloy_size end_len)
{
	alloy_size begin_len = 0;

	if (begin != ALLOY_NULL)
		begin_len = alloy_strlen(begin);

	alloy_size size = 0;
	size += begin_len;
	size += end_len;

	char *tmp = alloy_heap_realloc(&shell->heap, begin, size + 1);
	if (tmp == ALLOY_NULL)
		return ALLOY_NULL;

	for (alloy_size i = 0; i < end_len; i++)
		tmp[begin_len + i] = end[i];

	tmp[size] = 0;

	return tmp;
}

static char *shell_append_tok(struct AlloyShell *shell,
                              char *begin,
                              const struct AlloyToken *token)
{
	const char *end = token->buf;

	alloy_size end_len = token->buf_len;

	return shell_append(shell, begin, end, end_len);
}

static char *shell_append_var(struct AlloyShell *shell,
                              char *in,
                              const struct AlloyToken *token)
{
	const char *key = token->buf;

	alloy_size key_len = token->buf_len;

	const char *value = alloy_var_table_get_s(shell->var_table, key, key_len);
	if (value == ALLOY_NULL)
		return in;

	return shell_append(shell, in, value, alloy_strlen(value));
}

static char *shell_expand(struct AlloyShell *shell,
                          const char *in,
                          alloy_size in_len)
{
	char *out = ALLOY_NULL;

	struct AlloyScanner scanner;

	alloy_scanner_init(&scanner);

	alloy_scanner_set_buf(&scanner, in, in_len);

	while (!alloy_scanner_eof(&scanner))
	{
		struct AlloyToken *token = alloy_scanner_next(&scanner);
		if (token == ALLOY_NULL)
		{
			break;
		}
		else if ((token->id == ALLOY_TOKEN_SYMBOL)
		      && (token->buf_len == 1)
		      && (token->buf[0] == '$'))
		{
			token = alloy_scanner_next(&scanner);
			if (token == ALLOY_NULL)
			{
				/* TODO : ERROR */
				alloy_heap_free(&shell->heap, out);
				return ALLOY_NULL;
			}
			else if (token->id == ALLOY_TOKEN_IDENTIFIER)
			{
				char *tmp = shell_append_var(shell, out, token);
				if (tmp == ALLOY_NULL)
				{
					alloy_heap_free(&shell->heap, out);
					return ALLOY_NULL;
				}
				out = tmp;
			}
			else
			{
				char *tmp = shell_append_tok(shell, out, token);
				if (tmp == ALLOY_NULL)
				{
					alloy_heap_free(&shell->heap, out);
					return ALLOY_NULL;
				}
				out = tmp;
			}
		}
		else
		{
			char *tmp = shell_append_tok(shell, out, token);
			if (tmp == ALLOY_NULL)
			{
				alloy_heap_free(&shell->heap, out);
				return ALLOY_NULL;
			}
			out = tmp;
		}
	}

	return out;
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

	shell_write_z(shell, "> ");

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
	alloy_input_clear(shell->input);

	alloy_size offset = 0;
	alloy_size column = 0;

	struct AlloyTermPos pos;

	int err = shell_prompt(shell);
	if (err != 0)
		return err;

	err = alloy_term_get_cursor(shell->term, shell->term_data, &pos);
	if (err != 0)
		return err;

	column = pos.column;

	/* TODO : setting the cursor like this
	 * is a hack to make the vesaterm work.
	 * Eventually, this should be fixed in
	 * the vesaterm, so this line can be
	 * taken out.
	 * */
	err = alloy_term_set_cursor(shell->term, shell->term_data, &pos);
	if (err != 0)
		return err;

	for (;;)
	{
		alloy_utf8 c = 0;

		err = shell_get_char(shell, &c);
		if ((err != 0) || (c == ALLOY_KEY_QUIT))
			break;

		if (c == ALLOY_KEY_RETURN)
		{
			shell_write(shell, "\n", 1);
			break;
		}
		else if (c == ALLOY_KEY_BACKSPACE)
		{
			if (offset == 0)
				continue;

			alloy_input_backspace(shell->input);
			offset--;
		}
		else if (c == ALLOY_KEY_LEFT)
		{
			 if (offset == 0)
				continue;

			alloy_input_left(shell->input);
			offset--;
		}
		else if (c == ALLOY_KEY_RIGHT)
		{
			alloy_input_right(shell->input);
			offset++;
		}
		else if (c == ALLOY_KEY_UP)
		{
			/* TODO */
		}
		else if (c == ALLOY_KEY_DOWN)
		{
			/* TODO */
		}
		else
		{
			alloy_input_insert(shell->input, c);
			offset++;
		}

		err = shell_prompt(shell);
		if (err != 0)
			return err;

		pos.column = column + offset;

		err = alloy_term_set_cursor(shell->term, shell->term_data, &pos);
		if (err != 0)
			return err;
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

static int shell_define(struct AlloyShell *shell,
                        const struct AlloyToken *key_token,
                        const struct AlloyToken *value_token)
{
	char *key = alloy_heap_malloc(&shell->heap, key_token->buf_len + 1);
	if (key == ALLOY_NULL)
		return ALLOY_ENOMEM;

	for (alloy_size i = 0; i < key_token->buf_len; i++)
		key[i] = key_token->buf[i];

	key[key_token->buf_len] = 0;

	char *value = ALLOY_NULL;

	if (value_token != ALLOY_NULL)
	{
		value = alloy_heap_malloc(&shell->heap, value_token->buf_len + 1);
		if (value == ALLOY_NULL)
		{
			alloy_heap_free(&shell->heap, key);
			return ALLOY_ENOMEM;
		}

		for (alloy_size i = 0; i < value_token->buf_len; i++)
			value[i] = value_token->buf[i];

		value[value_token->buf_len] = 0;
	}

	int err = alloy_var_table_define(shell->var_table, key, value);

	alloy_heap_free(&shell->heap, key);
	alloy_heap_free(&shell->heap, value);

	if (err != 0)
		return err;

	return 0;
}

static int shell_try_assignment(struct AlloyShell *shell)
{
	int err = init_var_table(shell);
	if (err != 0)
		return err;

	struct AlloyScanner scanner;

	alloy_scanner_init(&scanner);

	alloy_scanner_set_buf(&scanner, shell->input->buf, shell->input->buf_len);

	alloy_scanner_ignore_space(&scanner);

	struct AlloyToken *tok = alloy_scanner_next(&scanner);
	if (tok == ALLOY_NULL)
		return ALLOY_EINVAL;

	struct AlloyToken name;
	name.id = tok->id;
	name.buf = tok->buf;
	name.buf_len = tok->buf_len;

	tok = alloy_scanner_next(&scanner);
	if ((tok == ALLOY_NULL)
	 || (tok->buf_len != 1)
	 || (tok->buf[0] != '='))
		return ALLOY_EINVAL;

	struct AlloyToken value;
	value.id = ALLOY_TOKEN_IDENTIFIER;
	value.buf = ALLOY_NULL;
	value.buf_len = 0;

	tok = alloy_scanner_next(&scanner);
	if (tok != ALLOY_NULL)
	{
		value.id = tok->id;
		value.buf = tok->buf;
		value.buf_len = tok->buf_len;
	}

	err = shell_define(shell, &name, &value);
	if (err != 0)
	{
		alloy_input_clear(shell->input);
		return err;
	}

	alloy_input_clear(shell->input);

	return 0;
}

static struct AlloyFile *shell_open(struct AlloyShell *shell,
                                    const char *path,
                                    enum AlloyFileMode mode)
{
	return alloy_host_open(shell->host, shell->host_data, path, mode);
}

static void shell_close(struct AlloyShell *shell,
                        struct AlloyFile *file)
{
	alloy_host_close(shell->host, shell->host_data, file);
}

static int shell_read(struct AlloyShell *shell,
                      struct AlloyFile *file,
                      void *buf,
                      alloy_size buf_size)
{
	return alloy_host_read(shell->host, shell->host_data, file, buf, buf_size);
}

static int cmd_version(struct AlloyShell *shell)
{
	shell_write_z(shell, ALLOY_VERSION_STRING);
	shell_write_z(shell, "\n");
	return 0;
}

static int cmd_help_print_cmd(struct AlloyShell *shell,
                              const char *name,
                              const char *desc)
{
	shell_write_z(shell, "  ");

	shell_set_foreground(shell, &shell->scheme.cmd_builtin);
	shell_write_z(shell, name);
	shell_set_foreground(shell, &shell->scheme.normal_foreground);

	shell_write_z(shell, " : ");
	shell_write_z(shell, desc);
	shell_write_z(shell, "\n");

	return 0;
}

static int cmd_help(struct AlloyShell *shell)
{
	shell_write_z(shell, "Commands:\n");
	cmd_help_print_cmd(shell, "cat    ", "Print file contents to screen.");
	cmd_help_print_cmd(shell, "clear  ", "Clear the screen.");
	cmd_help_print_cmd(shell, "color  ", "Color a component of the terminal.");
	cmd_help_print_cmd(shell, "dir    ", "List directory contents.");
	cmd_help_print_cmd(shell, "echo   ", "Echo content to the standard output.");
	cmd_help_print_cmd(shell, "exit   ", "Exit the shell.");
	cmd_help_print_cmd(shell, "help   ", "Get help with a command.");
	cmd_help_print_cmd(shell, "version", "Print this version of Alloy.");
	return 0;
}

static int cmd_cat_file(struct AlloyShell *shell,
                        struct AlloyFile *file)
{
	char buf[64];

	for (;;)
	{
		alloy_ssize read_size = shell_read(shell, file, buf, sizeof(buf));
		if (read_size <= 0)
			break;

		shell_write(shell, buf, (alloy_size) read_size);
	}

	return 0;
}

static int cmd_cat(struct AlloyShell *shell,
                   const struct AlloyCmd *cmd)
{
	for (alloy_size i = 1; i < cmd->argc; i++)
	{
		struct AlloyFile *file = shell_open(shell, cmd->argv[i], ALLOY_FILE_MODE_READ);
		if (file == ALLOY_NULL)
			continue;

		cmd_cat_file(shell, file);

		shell_close(shell, file);
	}

	return 0;
}

static int cmd_cd(struct AlloyShell *shell,
                  const struct AlloyCmd *cmd)
{
	const char *path = ALLOY_NULL;

	if (cmd->argc < 2)
	{
		path = "/Home";
	}
	else if (cmd->argc > 2)
	{
		shell_write_z(shell, "Trailing arguments found.\n");
		return ALLOY_EINVAL;
	}
	else
	{
		path = cmd->argv[1];
	}

	int err = alloy_shell_chdir(shell, path);
	if (err != 0)
	{
		shell_write_z(shell, "Failed to change directory to ");
		shell_set_foreground(shell, &shell->scheme.string_literal);
		shell_write_z(shell, "'");
		shell_write_z(shell, path);
		shell_write_z(shell, "'");
		shell_set_foreground(shell, &shell->scheme.normal_foreground);
		return err;
	}

	return 0;
}

static int cmd_clear(struct AlloyShell *shell)
{
	int err = alloy_term_clear(shell->term, shell->term_data);
	if (err != 0)
		return err;

	return 0;
}

static int cmd_color(struct AlloyShell *shell,
                     const struct AlloyCmd *cmd)
{
	if (cmd->argc < 2)
	{
		shell_write_z(shell, "No component name specified.\n");
		return ALLOY_EINVAL;
	}

	if (cmd->argc < 3)
	{
		shell_write_z(shell, "No color specified.\n");
		return ALLOY_EINVAL;
	}

	const char *color_str = cmd->argv[2];

	struct AlloyColor color;

	int err = alloy_color_parse(&color, color_str);
	if (err != 0)
	{
		shell_write_z(shell, "Invalid color value '");
		shell_write_z(shell, color_str);
		shell_write_z(shell, "'.\n");
		return ALLOY_EINVAL;
	}

	const char *component = cmd->argv[1];

	if (alloy_strcmp(component, "normal_foreground") == 0)
	{
		shell->scheme.normal_foreground = color;
	}
	else if (alloy_strcmp(component, "normal_background") == 0)
	{
		shell->scheme.normal_background = color;
	}
	else if (alloy_strcmp(component, "string_literal") == 0)
	{
		shell->scheme.string_literal = color;
	}
	else if (alloy_strcmp(component, "cmd_builtin") == 0)
	{
		shell->scheme.cmd_builtin = color;
	}
	else if (alloy_strcmp(component, "cmd_external") == 0)
	{
		shell->scheme.cmd_external = color;
	}
	else if (alloy_strcmp(component, "comment") == 0)
	{
		shell->scheme.comment = color;
	}
	else if (alloy_strcmp(component, "numerical") == 0)
	{
		shell->scheme.numerical = color;
	}
	else if (alloy_strcmp(component, "error") == 0)
	{
		shell->scheme.error = color;
	}
	else
	{
		shell_write_z(shell, "Invalid component name '");
		shell_write_z(shell, component);
		shell_write_z(shell, "'.\n");
		return ALLOY_EINVAL;
	}

	return 0;
}

static int cmd_dir(struct AlloyShell *shell,
                   struct AlloyCmd *cmd)
{
	const char *path = "/";

	if (cmd->argc > 1)
		path = cmd->argv[1];

	alloy_bool list_all = ALLOY_FALSE;

	struct AlloyDir *dir = shell_opendir(shell, path);

	if (dir == ALLOY_NULL)
	{
		shell_write_z(shell, "Failed to open ");

		shell_set_foreground(shell, &shell->scheme.string_literal);
		shell_write_z(shell, "'");
		shell_write_z(shell, path);
		shell_write_z(shell, "'");
		shell_set_foreground(shell, &shell->scheme.normal_foreground);

		shell_write_z(shell, "\n");

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
			shell_write_z(shell, "  file : ");
		else
			shell_write_z(shell, "  dir  : ");

		shell_set_foreground(shell, &shell->scheme.string_literal);
		shell_write_z(shell, "\"");
		shell_write_z(shell, ent->name);
		shell_write_z(shell, "\"");
		shell_set_foreground(shell, &shell->scheme.normal_foreground);
		shell_write_z(shell, "\n");
	}

	shell_closedir(shell, dir);

	return 0;
}

static int cmd_echo(struct AlloyShell *shell,
                    struct AlloyCmd *cmd)
{
	for (alloy_size i = 1; i < cmd->argc; i++)
	{
		shell_write_z(shell, cmd->argv[i]);

		if ((i + 1) < cmd->argc)
			shell_write_z(shell, " ");
	}

	shell_write_z(shell, "\n");

	return 0;
}

static int cmd_external(struct AlloyShell *shell,
                        struct AlloyCmd *cmd)
{
#ifndef ALLOY_WITH_BAREMETAL
	(void) shell;
	(void) cmd;
	return ALLOY_ENOSYS;
#endif

	if (cmd->argc == 0)
		return 0;

	struct AlloyFile *app_file = shell_open(shell, cmd->argv[0], ALLOY_FILE_MODE_READ);
	if (app_file == ALLOY_NULL)
		return ALLOY_ENOENT;

	struct AlloyHeap *heap = &shell->heap;

	unsigned char *app_data = 0;
	alloy_size app_size = 0;
	alloy_size app_res = 0;

	for (;;)
	{
		app_res += 4096;

		unsigned char *tmp = alloy_heap_realloc(heap, app_data, app_res);
		if (tmp == ALLOY_NULL)
		{
			alloy_heap_free(heap, app_data);
			return ALLOY_ENOMEM;
		}

		app_data = tmp;

		alloy_ssize read_size = shell_read(shell, app_file, &app_data[app_size], 4096);
		if (read_size == 0)
			break;
		else if (read_size < 0)
		{
			alloy_heap_free(heap, app_data);
			return ALLOY_EIO;
		}

		app_size += (alloy_size) read_size;
	}

	shell_close(shell, app_file);

	if (app_size == 0)
	{
		return ALLOY_EINVAL;
	}

	struct AlloyAppHost app_host;
	app_host.term = shell->term;
	app_host.term_data = shell->term_data;
	app_host.host = shell->host;
	app_host.host_data = shell->host_data;

	struct AlloyApp app;
	alloy_app_init(&app);
	alloy_app_set_host(&app, &app_host);
	alloy_app_set_args(&app, cmd->argc, cmd->argv);

	int err = alloy_app_load(&app, app_data, app_size);
	if (err != 0)
	{
		alloy_heap_free(heap, app_data);
		return err;
	}

	alloy_heap_free(heap, app_data);

	err = alloy_app_run(&app);
	if ((err != 0) && !shell->interactive)
	{
		/* TODO : fail out of a script. */
		return 0;
	}

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

	shell_write_z(shell, "Unknown command: ");

	shell_set_foreground(shell, &shell->scheme.string_literal);
	shell_write_z(shell, "'");
	shell_write_z(shell, cmd->argv[0]);
	shell_write_z(shell, "'");
	shell_set_foreground(shell, &shell->scheme.normal_foreground);

	shell_write_z(shell, ".\n");

	return 0;
}

static int shell_run_cmd(struct AlloyShell *shell)
{
	struct AlloyCmd cmd;
	alloy_cmd_init(&cmd);
	alloy_cmd_set_heap(&cmd, &shell->heap);

	char *in = shell_expand(shell, shell->input->buf, shell->input->buf_len);
	if (in == ALLOY_NULL)
	{
		alloy_cmd_done(&cmd);
		return ALLOY_ENOMEM;
	}

	int err = alloy_cmd_parse(&cmd, in);
	if (err != 0)
	{
		alloy_heap_free(&shell->heap, in);
		alloy_cmd_done(&cmd);
		alloy_input_clear(shell->input);
		return err;
	}

	alloy_heap_free(&shell->heap, in);

	alloy_input_clear(shell->input);

	err = 0;

	switch (cmd.id)
	{
	case ALLOY_CMD_CAT:
		err = cmd_cat(shell, &cmd);
		break;
	case ALLOY_CMD_CD:
		err = cmd_cd(shell, &cmd);
		break;
	case ALLOY_CMD_CLEAR:
		err = cmd_clear(shell);
		break;
	case ALLOY_CMD_COLOR:
		err = cmd_color(shell, &cmd);
		break;
	case ALLOY_CMD_DIR:
		err = cmd_dir(shell, &cmd);
		break;
	case ALLOY_CMD_EXIT:
		shell->quit_flag = ALLOY_TRUE;
		break;
	case ALLOY_CMD_ECHO:
		err = cmd_echo(shell, &cmd);
		break;
	case ALLOY_CMD_VERSION:
		err = cmd_version(shell);
		break;
	case ALLOY_CMD_HELP:
		err = cmd_help(shell);
		break;
	case ALLOY_CMD_UNKNOWN:

		err = cmd_external(shell, &cmd);
		if (err == ALLOY_ENOMEM)
		{
			shell_write_z(shell, "Not enough memory available for application.\n");
		}
		else if (err == ALLOY_EIO)
		{
			shell_write_z(shell, "Failed reading application from disk.\n");
		}
		else if (err == ALLOY_ENOENT)
		{
			err = cmd_unknown(shell, &cmd);
		}
		else if (err != 0)
		{
			shell_write_z(shell, "Failed to load application.\n");
		}

		break;
	}

	alloy_cmd_done(&cmd);

	return err;
}

static int shell_chdir_abs(struct AlloyShell *shell,
                           const char *path)
{
	(void) shell;
	(void) path;
	return ALLOY_ENOSYS;
}

static int shell_chdir_rel(struct AlloyShell *shell,
                           const char *path)
{
	(void) shell;
	(void) path;
	return ALLOY_ENOSYS;
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
	shell->var_table = ALLOY_NULL;
	shell->interactive = ALLOY_TRUE;
	shell->current_path = ALLOY_NULL;
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

	if (shell->var_table != ALLOY_NULL)
	{
		alloy_var_table_done(shell->var_table);
		alloy_heap_free(&shell->heap, shell->var_table);
		shell->var_table = ALLOY_NULL;
	}

	if (shell->host != ALLOY_NULL)
	{
		alloy_host_done(shell->host, shell->host_data);
		shell->host = ALLOY_NULL;
		shell->host_data = ALLOY_NULL;
	}
}

int alloy_shell_chdir(struct AlloyShell *shell,
                      const char *path)
{
	if (path == ALLOY_NULL)
		return ALLOY_EFAULT;
	else if (path[0] == '/')
		return shell_chdir_abs(shell, path);
	else
		return shell_chdir_rel(shell, path);
}

int alloy_shell_run(struct AlloyShell *shell)
{
	/* Ensure that the terminal is initialized */
	int err = init_term(shell);
	if (err != 0)
		return err;

	/* Ensure that the input is initialized. */
	err = init_input(shell);
	if (err != 0)
		return err;

	/* Clear the terminal to ensure that the
	 * current theme presents itself. */
	err = alloy_term_clear(shell->term, shell->term_data);
	if (err != 0)
		return err;

	while (!shell->quit_flag)
	{
		/* Ensure that the input buffer
		 * does not contain anything in it. */
		alloy_input_clear(shell->input);

		err = alloy_shell_run_once(shell);
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
		shell_write_z(shell, "Failed to initialize input.\n");
		return err;
	}

	err = shell_get_line(shell);
	if (err != 0)
	{
		shell_write_z(shell, "Failed to read input line.\n");
		return err;
	}

	shell_set_foreground(shell, &shell->scheme.normal_foreground);
	shell_set_background(shell, &shell->scheme.normal_background);

	err = shell_try_assignment(shell);
	if (err != 0)
	{
		err = shell_run_cmd(shell);
		if ((err != 0) && (!shell->interactive))
		{
			shell_write_z(shell, "Failed to run command.\n");
			return err;
		}
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

int alloy_shell_write_z(struct AlloyShell *shell,
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
