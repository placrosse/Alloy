#ifndef ALLOY_TERM_H
#define ALLOY_TERM_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyTerm
{
	/** Used for passing data to the
	 * term implementation. */
	void *data;
	/** Called when the term is no longer
	 * going to be used by the caller. */
	void (*done)(void *data);
	/** Called to clear the screen and
	 * reset the cursor position. */
	int (*clear)(void *data);
	/** Gets the width and height, in character spaces, of the terminal. */
	int (*get_size)(void *data, unsigned int *width, unsigned int *height);
	/** Writes text to the screen. */
	int (*write)(void *data, const char *str, unsigned int str_len);
	/** Sets the background color. */
	int (*set_background)(void *data, unsigned long int color);
	/** Sets the cursor position. */
	int (*set_cursor)(void *data, unsigned int line, unsigned int column);
	/** Sets the foreground color. */
	int (*set_foreground)(void *data, unsigned long int color);
	/** Toggles the cursor visibility. */
	int (*toggle_cursor)(void *data);
};

void alloy_term_init(struct AlloyTerm *term);

void alloy_term_done(struct AlloyTerm *term);

int alloy_term_clear(struct AlloyTerm *term);

int alloy_term_get_size(struct AlloyTerm *term,
                        unsigned int *width,
                        unsigned int *height);

int alloy_term_toggle_cursor_visibility(struct AlloyTerm *term);

int alloy_term_set_background(struct AlloyTerm *term,
                              unsigned long int color);

int alloy_term_set_cursor(struct AlloyTerm *term,
                          unsigned int line,
                          unsigned int column);

int alloy_term_set_foreground(struct AlloyTerm *term,
                              unsigned long int color);

int alloy_term_write(struct AlloyTerm *term,
                     const char *str,
                     unsigned int str_len);

int alloy_term_write_asciiz(struct AlloyTerm *term,
                            const char *str);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_TERM_H */
