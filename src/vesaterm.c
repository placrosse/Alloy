#include <alloy/vesaterm.h>

#include <stdint.h>

void alloy_vesaterm_init(struct AlloyVesaTerm *term)
{
	alloy_term_init(&term->base);
	term->x_res = *(uint16_t *)(0x5084);
	term->y_res = *(uint16_t *)(0x5086);
	term->depth = *(uint8_t *)(0x5088);
	term->frame_buffer = (unsigned char *)((uint64_t)(*(uint32_t *)(0x5080)));
}

