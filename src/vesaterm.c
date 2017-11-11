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

	for (unsigned int y = 0; y < glyph->height; y++)
	{
		for (unsigned int x = 0; x < glyph->width; x++)
		{
			unsigned int glyph_offset = 0;
			glyph_offset += y * glyph->width;
			glyph_offset += x;

			unsigned char intensity = glyph->buf[glyph_offset];

			unsigned buffer_offset = 0;
			buffer_offset += (y + term->y_pos + alloy_font.line_height - glyph->top) * term->x_res;
			buffer_offset += x + term->x_pos + glyph->left;
			buffer_offset *= term->depth / 8;

			term->frame_buffer[buffer_offset + 0] = intensity;
			term->frame_buffer[buffer_offset + 1] = intensity;
			term->frame_buffer[buffer_offset + 2] = intensity;
		}
	}

	term->x_pos += glyph->advance;
	term->column++;

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
	term->base.write = vesaterm_write;
	term->tab_width = 8;
	term->line = 1;
	term->column = 1;
	/* initialize foreground to green */
	term->foreground = 0x00ff00;
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

