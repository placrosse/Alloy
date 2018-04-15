/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "term.h"

#include <alloy/color.h>
#include <alloy/errno.h>
#include <alloy/keys.h>
#include <alloy/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

struct AlloyTermData
{
	alloy_size tab_size;
	FILE *out;
	FILE *in;
	struct AlloyCursorPos pos;
	struct termios original_attr;
};

static struct AlloyTermData *xterm_init(void)
{
	struct AlloyTermData *term_data = malloc(sizeof(*term_data));
	if (term_data == NULL)
		return ALLOY_NULL;

	term_data->out = stdout;
	term_data->in = stdin;
	term_data->pos.line = 1;
	term_data->pos.column = 1;

	/* TODO : get tab size from terminal. */
	term_data->tab_size = 8;

	/* Get a copy of the terminal
	 * attributes so we can restore them
	 * on exit. */
	tcgetattr(1, &term_data->original_attr);

	/* Disable input echoing */
	struct termios t;
	tcgetattr(1, &t);
	t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(1, TCSANOW, &t);

	return term_data;
}

static void xterm_done(struct AlloyTermData *term_data)
{
	if (term_data == ALLOY_NULL)
		return;

	tcsetattr(1, TCSANOW, &term_data->original_attr);

	free(term_data);
}

static int xterm_get_char(struct AlloyTermData *term_data,
                          alloy_utf8 *c_ptr)
{
	alloy_utf8 c = 0;

	if (fread(&c, 1, 1, term_data->in) != 1)
		return ALLOY_EIO;

	if (c == 0x7f)
	{
		c = ALLOY_KEY_BACKSPACE;
	}
	else if (c == 0x1b)
	{
		if (fread(&c, 1, 1, term_data->in) != 1)
			return ALLOY_EIO;

		if (c != '[')
		{
			/* This is the only control sequence
			 * that is understood. */
			return ALLOY_EINVAL;
		}

		if (fread(&c, 1, 1, term_data->in) != 1)
			return ALLOY_EIO;

		if (c == 'D')
			c = ALLOY_KEY_LEFT;
		else if (c == 'C')
			c = ALLOY_KEY_RIGHT;
		else if (c == 'B')
			c = ALLOY_KEY_DOWN;
		else if (c == 'A')
			c = ALLOY_KEY_UP;
		else
			return ALLOY_EINVAL;
	}

	if (c_ptr != ALLOY_NULL)
		*c_ptr = c;

	return 0;
}

static int xterm_get_cursor(struct AlloyTermData *term_data,
                            struct AlloyCursorPos *pos)
{
	pos->line = term_data->pos.line;
	pos->column = term_data->pos.column;
	return 0;
}

static int xterm_get_size(struct AlloyTermData *term_data,
                          struct AlloyTermSize *term_size)
{
	(void) term_data;
	term_size->height = 180;
	term_size->width = 120;
	return 0;
}

static int xterm_set_background(struct AlloyTermData *term_data,
                                const struct AlloyColor *color)
{
	if (term_data == ALLOY_NULL)
		return ALLOY_EFAULT;

	fprintf(term_data->out,
		"\x1b[48;2;%u;%u;%um",
		color->red,
		color->green,
		color->blue);

	return 0;
}

static int xterm_set_cursor(struct AlloyTermData *term_data,
                            const struct AlloyCursorPos *pos)
{
	term_data->pos.line = pos->line;
	term_data->pos.column = pos->column;

	fprintf(term_data->out,
	        "\x1b[%u;%uH\n",
	        (unsigned int) pos->line,
	        (unsigned int) pos->column);

	return 0;
}

static int xterm_clear(struct AlloyTermData *term_data)
{
	printf("\x1b[2J\x1b[H");

	term_data->pos.column = 1;
	term_data->pos.line = 1;

	return 0;
}

static int xterm_set_foreground(struct AlloyTermData *term_data,
                                const struct AlloyColor *color)
{
	if (term_data == ALLOY_NULL)
		return ALLOY_EFAULT;

	fprintf(term_data->out,
		"\x1b[38;2;%u;%u;%um",
		color->red,
		color->green,
		color->blue);

	return 0;
}

static int xterm_write(struct AlloyTermData *term_data,
                       const char *str,
                       alloy_size str_size)
{
	for (alloy_size i = 0; i < str_size; i++)
	{
		char c = str[i];
		if (c == '\t')
		{
			term_data->pos.column += term_data->tab_size - (term_data->pos.column % term_data->tab_size);
			fputc(c, term_data->out);
		}
		else if (c == '\n')
		{
			term_data->pos.line++;
			term_data->pos.column = 1;
			fputc(c, term_data->out);
		}
		else if (c == '\r')
		{
			term_data->pos.column = 1;
			fputc(c, term_data->out);
		}
		else
		{
			fputc(c, term_data->out);
			term_data->pos.column++;
		}
	}

	return 0;
}

const struct AlloyTerm alloy_term = {
	xterm_init,
	xterm_done,
	xterm_clear,
	xterm_get_char,
	xterm_get_cursor,
	xterm_get_size,
	xterm_write,
	xterm_set_background,
	xterm_set_cursor,
	xterm_set_foreground,
};
