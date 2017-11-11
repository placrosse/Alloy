#ifndef ALLOY_VESATERM_H
#define ALLOY_VESATERM_H

#include <alloy/term.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** A terminal implementation using
 * VESA graphics.
 */

struct AlloyVesaTerm
{
	/** The base structure of the terminal
	 * implementation.*/
	struct AlloyTerm base;
	/** Foreground color, in xxRRGGBB pixel format */
	unsigned long int foreground;
	/** Background color, in xxRRGGBB pixel format */
	unsigned long int background;
	/** The number of pixels per line. */
	unsigned int x_res;
	/** The number of lines in a frame. */
	unsigned int y_res;
	/** The number of bits per pixel. */
	unsigned int depth;
	/** A poitner to the frame memory contents. */
	unsigned char *frame_buffer;
};

/** Initializes the contents of the
 * terminal structure.
 */

void alloy_vesaterm_init(struct AlloyVesaTerm *term);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_VESATERM_H */
