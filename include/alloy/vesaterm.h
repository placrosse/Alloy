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
	/** The number of spaces that a tab should
	 * occupy. */
	unsigned int tab_width;
	/** The current line that the cursor is on.
	 * The first line of the terminal is equal to
	 * one. */
	unsigned int line;
	/** The current column that the cursor is on.
	 * The first column of the terminal is equal
	 * to one. */
	unsigned int column;
	/** The horizontal position of the 'pen', in pixels. */
	unsigned int x_pos;
	/** The vertical position of the 'pen', in pixels. */
	unsigned int y_pos;
	/** The number of pixels per line. */
	unsigned int x_res;
	/** The number of lines in a frame. */
	unsigned int y_res;
	/** The number of bits per pixel. */
	unsigned int depth;
	/** A pointer to the frame memory contents. */
	unsigned char *frame_buffer;
	/** A temporary buffer for reading screen contents. */
	unsigned char *swap_buffer;
};

/** Initializes the contents of the
 * terminal structure.
 */

void alloy_vesaterm_init(struct AlloyVesaTerm *term);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_VESATERM_H */
