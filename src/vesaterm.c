#include <alloy/vesaterm.h>

#include <stdint.h>
#include <stdlib.h>

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
	(void) term_ptr;
	(void) str;
	(void) str_len;
	return 0;
}

void alloy_vesaterm_init(struct AlloyVesaTerm *term)
{
	alloy_term_init(&term->base);
	term->base.data = term;
	term->base.done = vesaterm_done;
	term->base.clear = vesaterm_clear;
	term->base.write = vesaterm_write;
	/* initialize foreground to green */
	term->foreground = 0x00ff00;
	/* initialize background to black */
	term->background = 0x000000;
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

