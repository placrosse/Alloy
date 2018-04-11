#ifndef ALLOY_TERM_H
#define ALLOY_TERM_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyTermData;

struct AlloyCursorPos
{
	unsigned int line;
	unsigned int column;
};

struct AlloyTermSize
{
	unsigned int width;
	unsigned int height;
};

struct AlloyColor
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

struct AlloyTerm
{
	struct AlloyTermData *(*init)(void);
	/** Called when the term is no longer
	 * going to be used by the caller. */
	void (*done)(struct AlloyTermData *term_data);
	/** Called to clear the screen and
	 * reset the cursor position. */
	int (*clear)(struct AlloyTermData *term_data);
	/** Gets the current cursor position. */
	int (*get_cursor)(struct AlloyTermData *term_data,
	                  struct AlloyCursorPos *cursor_pos);
	/** Gets the width and height, in character spaces, of the terminal. */
	int (*get_size)(struct AlloyTermData *term_data,
	                struct AlloyTermSize *term_size);
	/** Writes text to the screen. */
	int (*write)(struct AlloyTermData *term_data,
	             const char *str, unsigned int str_len);
	/** Sets the background color. */
	int (*set_background)(struct AlloyTermData *term_data,
	                      const struct AlloyColor *background_color);
	/** Sets the cursor position. */
	int (*set_cursor)(struct AlloyTermData *term_data,
	                  const struct AlloyCursorPos *cursor_pos);
	/** Sets the foreground color. */
	int (*set_foreground)(struct AlloyTermData *term_data,
	                      const struct AlloyColor *foreground);
};

struct AlloyTermData *alloy_term_init(const struct AlloyTerm *term);

void alloy_term_done(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data);

int alloy_term_clear(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data);

int alloy_term_clear_line(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data);

int alloy_term_get_cursor(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data,
                          struct AlloyCursorPos *cursor_pos);

int alloy_term_get_size(const struct AlloyTerm *term,
                        struct AlloyTermData *term_data,
                        struct AlloyTermSize *term_size);

int alloy_term_set_background(const struct AlloyTerm *term,
                              struct AlloyTermData *term_data,
                              const struct AlloyColor *color);

int alloy_term_set_cursor(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data,
                          const struct AlloyCursorPos *pos);

int alloy_term_set_foreground(const struct AlloyTerm *term,
                              struct AlloyTermData *term_data,
                              const struct AlloyColor *color);

int alloy_term_write(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data,
                     const char *str,
                     unsigned int str_len);

int alloy_term_write_asciiz(const struct AlloyTerm *term,
                            struct AlloyTermData *term_data,
                            const char *str);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_TERM_H */
