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
	term->base.write = vesaterm_write;
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

