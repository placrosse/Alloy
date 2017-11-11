#ifndef ALLOY_TERM_H
#define ALLOY_TERM_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyTerm
{
	/** Used for passing data to the
	 * terminal implementation. */
	void *data;
	/** Called when the terminal is no longer
	 * going to be used by the caller. */
	void (*done)(void *data);
	/** Writes text to the screen. */
	int (*write)(void *data, const char *str, unsigned int str_len);
	/** Sets the background color. */
	int (*set_background)(void *data, unsigned long int color);
	/** Sets the foreground color. */
	int (*set_foreground)(void *data, unsigned long int color);
};

void alloy_term_init(struct alloy_terminal *terminal);

void alloy_term_done(struct alloy_terminal *terminal);

int alloy_term_write(struct alloy_term *term,
                     const char *str,
                     unsigned int str_len);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_TERM_H */
