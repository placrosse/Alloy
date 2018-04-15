/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "term.h"

#include <alloy/color.h>
#include <alloy/errno.h>
#include <alloy/font.h>
#include <alloy/keys.h>

#include <baremetal/syscalls.h>

#ifndef BAREMETAL_PAGE_SIZE
#define BAREMETAL_PAGE_SIZE (2 * 1024 * 1024)
#endif

struct AlloyTermData
{
	struct AlloyColor foreground;
	struct AlloyColor background;
	alloy_size tab_width;
	alloy_size line;
	alloy_size column;
	alloy_size x_pos;
	alloy_size y_pos;
	alloy_size x_res;
	alloy_size y_res;
	alloy_size depth;
	alloy_uint8 *frame_buffer;
	alloy_uint8 *swap_buffer;
};

static int vesaterm_scroll_down(struct AlloyTermData *term)
{
	unsigned char *dst = (unsigned char *) term->frame_buffer;

	unsigned char *dst_swap = (unsigned char *) term->swap_buffer;

	unsigned char *src = &dst_swap[alloy_font.line_height * term->x_res * term->depth / 8];

	for (unsigned int y = 0; y < (term->y_res - alloy_font.line_height); y++)
	{
		for (unsigned int x = 0; x < term->x_res; x++)
		{
			unsigned int buf_offset = 0;
			buf_offset += y * term->x_res;
			buf_offset += x;
			buf_offset *= term->depth / 8;

			dst[buf_offset + 0] = src[buf_offset + 0];
			dst[buf_offset + 1] = src[buf_offset + 1];
			dst[buf_offset + 2] = src[buf_offset + 2];

			dst_swap[buf_offset + 0] = src[buf_offset + 0];
			dst_swap[buf_offset + 1] = src[buf_offset + 1];
			dst_swap[buf_offset + 2] = src[buf_offset + 2];
		}
	}

	return alloy_term_clear_line(&alloy_term, term);
}

static int vesaterm_write_char(struct AlloyTermData *term, char c)
{
	if (c == '\n')
	{
		term->column = 1;

		term->x_pos = 0;

		/* check if we can just scroll down instead of moving
		 * the cursor downwards */
		if ((term->y_pos + (alloy_font.line_height * 2)) >= term->y_res)
			return vesaterm_scroll_down(term);

		term->line++;

		term->y_pos += alloy_font.line_height;

		return 0;
	}
	else if (c == '\t')
	{
		while (term->column % term->tab_width)
		{
			int err = vesaterm_write_char(term, ' ');
			if (err != 0)
				return err;
		}
		return 0;
	}

	const struct AlloyGlyph *glyph = alloy_font_get_glyph(&alloy_font, c);
	if (glyph == ALLOY_NULL)
		return ALLOY_EINVAL;

	/* color in the background */

	for (unsigned int y = 0; (y < (alloy_font.ascender - alloy_font.descender)) && ((y + term->y_pos) < term->y_res); y++)
	{
		unsigned int background_width = glyph->advance;
		for (unsigned int x = 0; (x < background_width) && ((x + term->x_pos) < term->x_res); x++)
		{
			unsigned buffer_offset = 0;
			buffer_offset += (y + term->y_pos) * term->x_res;
			buffer_offset += x + term->x_pos;
			buffer_offset *= term->depth / 8;

			unsigned long int red = term->background.red;
			unsigned long int green = term->background.green;
			unsigned long int blue = term->background.blue;

			term->frame_buffer[buffer_offset + 0] = blue;
			term->frame_buffer[buffer_offset + 1] = green;
			term->frame_buffer[buffer_offset + 2] = red;

			term->swap_buffer[buffer_offset + 0] = blue;
			term->swap_buffer[buffer_offset + 1] = green;
			term->swap_buffer[buffer_offset + 2] = red;
		}
	}

	/* color in the foreground */

	for (unsigned int y = 0; (y < glyph->height) && ((y + term->y_pos + alloy_font.ascender - glyph->top) < term->y_res); y++)
	{

		for (unsigned int x = 0; (x < glyph->width) && ((x + term->x_pos + glyph->left) < term->x_res); x++)
		{
			unsigned int glyph_offset = 0;
			glyph_offset += y * glyph->width;
			glyph_offset += x;

			unsigned int intensity = glyph->buf[glyph_offset];

			unsigned buffer_offset = 0;
			buffer_offset += (y + term->y_pos + alloy_font.ascender - glyph->top) * term->x_res;
			buffer_offset += x + term->x_pos + glyph->left;
			buffer_offset *= term->depth / 8;

			unsigned long int red = 0;
			red += intensity * term->foreground.red;
			red += (255 - intensity) * term->background.red;
			red /= 0x100;

			unsigned long int green = 0;
			green += intensity * term->foreground.green;
			green += (255 - intensity) * term->background.green;
			green /= 0x100;

			unsigned long int blue = 0;
			blue += intensity * term->foreground.blue;
			blue += (255 - intensity) * term->background.blue;
			blue /= 0x100;

			term->frame_buffer[buffer_offset + 0] = blue;
			term->frame_buffer[buffer_offset + 1] = green;
			term->frame_buffer[buffer_offset + 2] = red;

			term->swap_buffer[buffer_offset + 0] = blue;
			term->swap_buffer[buffer_offset + 1] = green;
			term->swap_buffer[buffer_offset + 2] = red;
		}
	}

	term->x_pos += glyph->advance;
	term->column++;

	return 0;
}

static int vesaterm_get_char(struct AlloyTermData *term,
                             alloy_utf8 *c_ptr)
{
	(void) term;

	alloy_utf8 c = 0;

	for (;;)
	{
		c = b_input_key();
		if (c != 0)
			break;
	}

	if (c == 0x0e)
		c = ALLOY_KEY_BACKSPACE;
	else if (c == 0x1c)
		c = ALLOY_KEY_RETURN;

	if (c_ptr != ALLOY_NULL)
		*c_ptr = c;

	return 0;
}

static int vesaterm_get_cursor(struct AlloyTermData *term,
                               struct AlloyCursorPos *pos)
{
	pos->line = term->line;
	pos->column = term->column;
	return 0;
}

static int vesaterm_get_size(struct AlloyTermData *term,
                             struct AlloyTermSize *size)
{
	/* Calculate the width and space of the
	 * terminal by dividing the x and y resolutions
	 * by the space character width and line
	 * height (respectively).
	 * */

	const struct AlloyGlyph *space_glyph = alloy_font_get_glyph(&alloy_font, ' ');
	if (space_glyph == ALLOY_NULL)
		return -1;

	size->width = term->x_res / space_glyph->advance;

	size->height = term->y_res / alloy_font.line_height;

	return 0;
}

static int vesaterm_set_background(struct AlloyTermData *term,
                                   const struct AlloyColor *background)
{
	term->background = *background;
	return 0;
}

static int vesaterm_set_cursor(struct AlloyTermData *term,
                               const struct AlloyCursorPos *pos)
{
	/* column and line must be greater than zero */

	if ((pos->line <= 0) || (pos->column <= 0))
		return -1;

	term->line = pos->line;
	term->column = pos->column;

	/* determine the new x and y positions by
	 * using the width of a space character and
	 * the line height.
	 */

	const struct AlloyGlyph *space_glyph = alloy_font_get_glyph(&alloy_font, ' ');
	if (space_glyph == ALLOY_NULL)
		return -1;

	term->x_pos = (pos->column - 1) * space_glyph->advance;

	term->y_pos = (pos->line - 1) * alloy_font.line_height;

	return 0;
}

static int vesaterm_set_foreground(struct AlloyTermData *term,
                                   const struct AlloyColor *foreground)
{
	term->foreground = *foreground;
	return 0;
}

static void vesaterm_done(struct AlloyTermData *term)
{
#ifdef ALLOY_WITH_BAREMETAL
	/* nothing to do here */
	(void) term;
#else /* ALLOY_WITH_BAREMETAL */
	if (term->frame_buffer != NULL)
	{
		free(term->frame_buffer);
		term->frame_buffer = NULL;
	}
#endif /* ALLOY_WITH_BAREMETAL */
}

static int vesaterm_clear(struct AlloyTermData *term)
{
	for (unsigned int y = 0; y < term->y_res; y++)
	{
		for (unsigned int x = 0; x < term->x_res; x++)
		{
			unsigned int byte_offset = 0;
			/* vertical offset */
			byte_offset += y * term->x_res;
			/* horizontal offset */
			byte_offset += x;
			/* byte offset */
			byte_offset *= (term->depth / 8);
			/* set the background red pixel */
			term->frame_buffer[byte_offset + 0] = term->background.blue;
			/* set the background green pixel */
			term->frame_buffer[byte_offset + 1] = term->background.green;
			/* set the background blue pixel */
			term->frame_buffer[byte_offset + 2] = term->background.red;
		}
	}

	/* restore cursor position */

	term->x_pos = 0;
	term->y_pos = 0;
	term->line = 1;
	term->column = 1;

	return 0;
}

static int vesaterm_write(struct AlloyTermData *term,
                          const char *str,
                          alloy_size str_len)
{
	for (alloy_size i = 0; i < str_len; i++)
	{
		int err = vesaterm_write_char(term, str[i]);
		if (err != 0)
			return err;
	}

	return 0;
}

struct AlloyTermData global_term;

struct AlloyTermData *vesaterm_init(void)
{
	struct AlloyTermData *term = &global_term;
	term->tab_width = 8;
	term->line = 1;
	term->column = 1;
	term->foreground = alloy_white;
	term->background = alloy_black;
	term->x_pos = 0;
	term->y_pos = 0;
#ifdef ALLOY_WITH_BAREMETAL
	term->x_res = *(alloy_uint16 *)(0x5084);
	term->y_res = *(alloy_uint16 *)(0x5086);
	term->depth = *(alloy_uint8 *)(0x5088);
	term->frame_buffer = (alloy_uint8 *)((alloy_uint64)(*(alloy_uint32 *)(0x5080)));
	// TODO : calculate the number 
	alloy_size page_count = 0;
	page_count += term->x_res * term->y_res;
	page_count *= term->depth / 8;
	page_count += BAREMETAL_PAGE_SIZE;
	page_count /= BAREMETAL_PAGE_SIZE;
	b_mem_allocate((void **) &term->swap_buffer, page_count);
#else /* ALLOY_WITH_BAREMETAL */
	term->x_res = 1024;
	term->y_res = 768;
	term->depth = 24;
	term->frame_buffer = malloc(term->x_res * term->y_res * (term->depth / 8));
	term->swap_buffer = NULL;
#endif /* ALLOY_WITH_BAREMETAL */
	return term;
}

const struct AlloyTerm alloy_term = {
	vesaterm_init,
	vesaterm_done,
	vesaterm_clear,
	vesaterm_get_char,
	vesaterm_get_cursor,
	vesaterm_get_size,
	vesaterm_write,
	vesaterm_set_background,
	vesaterm_set_cursor,
	vesaterm_set_foreground,
};
