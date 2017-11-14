#include <alloy/vesaterm.h>

#include <alloy/font.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

static int vesaterm_write_char(struct AlloyVesaTerm *term, char c)
{
	if (c == '\n')
	{
		term->y_pos += alloy_font.line_height;
		term->x_pos = 0;
		term->line++;
		term->column = 1;
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
	if (glyph == NULL)
		/* TODO : better error code ? */
		return -EINVAL;

	/* color in the background */

	for (unsigned int y = 0; (y < alloy_font.line_height) && ((y + term->y_pos) < term->y_res); y++)
	{
		unsigned int background_width = glyph->advance;
		for (unsigned int x = 0; (x < background_width) && ((x + term->x_pos) < term->x_res); x++)
		{
			unsigned buffer_offset = 0;
			buffer_offset += (y + term->y_pos) * term->x_res;
			buffer_offset += x + term->x_pos;
			buffer_offset *= term->depth / 8;

			unsigned long int red   = (term->background & 0xff0000) >> 0x10;
			unsigned long int green = (term->background & 0x00ff00) >> 0x08;
			unsigned long int blue  = (term->background & 0x0000ff) >> 0x00;

			term->frame_buffer[buffer_offset + 0] = red;
			term->frame_buffer[buffer_offset + 1] = green;
			term->frame_buffer[buffer_offset + 2] = blue;
		}
	}

	/* color in the foreground */

	for (unsigned int y = 0; (y < glyph->height) && ((y + term->y_pos + alloy_font.line_height - glyph->top) < term->y_res); y++)
	{

		for (unsigned int x = 0; (x < glyph->width) && ((x + term->x_pos + glyph->left) < term->x_res); x++)
		{
			unsigned int glyph_offset = 0;
			glyph_offset += y * glyph->width;
			glyph_offset += x;

			unsigned int intensity = glyph->buf[glyph_offset];

			unsigned buffer_offset = 0;
			buffer_offset += (y + term->y_pos + alloy_font.line_height - glyph->top) * term->x_res;
			buffer_offset += x + term->x_pos + glyph->left;
			buffer_offset *= term->depth / 8;

			unsigned long int red = 0;
			red += intensity * ((term->foreground & 0xff0000) >> 0x10);
			red += (255 - intensity) * ((term->background & 0xff0000) >> 0x10);
			red /= 0x100;

			unsigned long int green = 0;
			green += intensity * ((term->foreground & 0xff00) >> 0x08);
			green += (255 - intensity) * ((term->background & 0xff00) >> 0x08);
			green /= 0x100;

			unsigned long int blue = 0;
			blue += intensity * (term->foreground & 0xff);
			blue += (255 - intensity) * (term->background & 0xff);
			blue /= 0x100;

			term->frame_buffer[buffer_offset + 0] = red;
			term->frame_buffer[buffer_offset + 1] = green;
			term->frame_buffer[buffer_offset + 2] = blue;
		}
	}

	term->x_pos += glyph->advance;
	term->column++;

	return 0;
}

static int vesaterm_get_cursor(void *term_ptr,
                               unsigned int *line,
                               unsigned int *column)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;

	*line = term->line;
	*column = term->column;

	return 0;
}

static int vesaterm_get_size(void *term_ptr,
                             unsigned int *width,
                             unsigned int *height)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;

	/* Calculate the width and space of the
	 * terminal by dividing the x and y resolutions
	 * by the space character width and line
	 * height (respectively).
	 * */

	const struct AlloyGlyph *space_glyph = alloy_font_get_glyph(&alloy_font, ' ');
	if (space_glyph == NULL)
		return -1;

	*width = term->x_res / space_glyph->advance;

	*height = term->y_res / alloy_font.line_height;

	return 0;
}

static int vesaterm_set_background(void *term_ptr,
                                   unsigned long int color)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;
	term->background = color;
	return 0;
}

static int vesaterm_set_cursor(void *term_ptr,
                               unsigned int line,
                               unsigned int column)
{
	/* column and line must be greater than zero */

	if ((line <= 0) || (column <= 0))
		return -1;

	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;
	term->line = line;
	term->column = column;

	/* determine the new x and y positions by
	 * using the width of a space character and
	 * the line height.
	 */

	const struct AlloyGlyph *space_glyph = alloy_font_get_glyph(&alloy_font, ' ');
	if (space_glyph == NULL)
		return -1;

	term->x_pos = (column - 1) * space_glyph->advance;

	term->y_pos = (line - 1) * alloy_font.line_height;

	return 0;
}

static int vesaterm_set_foreground(void *term_ptr,
                                   unsigned long int color)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;
	term->foreground = color;
	return 0;
}

static void vesaterm_done(void *term_ptr)
{
#ifdef ALLOY_WITH_BAREMETAL
	/* nothing to do here */
	(void) term_ptr;
#else /* ALLOY_WITH_BAREMETAL */
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;
	if (term->frame_buffer != NULL)
	{
		free(term->frame_buffer);
		term->frame_buffer = NULL;
	}
#endif /* ALLOY_WITH_BAREMETAL */
}

static int vesaterm_clear(void *term_ptr)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;

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
			term->frame_buffer[byte_offset + 0] = (term->background & 0xff0000) >> 16;
			/* set the background green pixel */
			term->frame_buffer[byte_offset + 1] = (term->background & 0x00ff00) >> 8;
			/* set the background blue pixel */
			term->frame_buffer[byte_offset + 2] = (term->background & 0x0000ff) >> 0;
		}
	}

	/* restore cursor position */

	term->x_pos = 0;
	term->y_pos = 0;
	term->line = 1;
	term->column = 1;

	return 0;
}

static int vesaterm_write(void *term_ptr,
                          const char *str,
                          unsigned int str_len)
{
	struct AlloyVesaTerm *term = (struct AlloyVesaTerm *) term_ptr;

	for (unsigned int i = 0; i < str_len; i++)
	{
		int err = vesaterm_write_char(term, str[i]);
		if (err != 0)
			return err;
	}
	return 0;
}

void alloy_vesaterm_init(struct AlloyVesaTerm *term)
{
	alloy_term_init(&term->base);
	term->base.data = term;
	term->base.done = vesaterm_done;
	term->base.clear = vesaterm_clear;
	term->base.get_cursor = vesaterm_get_cursor;
	term->base.get_size = vesaterm_get_size;
	term->base.set_background = vesaterm_set_background;
	term->base.set_cursor = vesaterm_set_cursor;
	term->base.set_foreground = vesaterm_set_foreground;
	term->base.write = vesaterm_write;
	term->tab_width = 8;
	term->line = 1;
	term->column = 1;
	/* initialize foreground to white */
	term->foreground = 0xffffff;
	/* initialize background to black */
	term->background = 0x000000;
	term->x_pos = 0;
	term->y_pos = 0;
#ifdef ALLOY_WITH_BAREMETAL
	term->x_res = *(uint16_t *)(0x5084);
	term->y_res = *(uint16_t *)(0x5086);
	term->depth = *(uint8_t *)(0x5088);
	term->frame_buffer = (unsigned char *)((uint64_t)(*(uint32_t *)(0x5080)));
#else /* ALLOY_WITH_BAREMETAL */
	term->x_res = 1024;
	term->y_res = 768;
	term->depth = 24;
	term->frame_buffer = malloc(term->x_res * term->y_res * (term->depth / 8));
#endif /* ALLOY_WITH_BAREMETAL */
}

