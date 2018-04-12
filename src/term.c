#include <alloy/term.h>

#include <alloy/errno.h>
#include <alloy/types.h>

struct AlloyTermData *alloy_term_init(const struct AlloyTerm *term)
{
	if (term->init == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return term->init();
}

void alloy_term_done(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data)
{
	if (term->done != ALLOY_NULL)
		term->done(term_data);
}

int alloy_term_clear(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data)
{
	if (term->clear == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->clear(term_data);
}

int alloy_term_clear_line(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data)
{
	struct AlloyTermSize term_size;

	int err = alloy_term_get_size(term, term_data, &term_size);
	if (err != 0)
		return err;

	unsigned int width = term_size.width;

	struct AlloyCursorPos original_pos;

	err = alloy_term_get_cursor(term, term_data, &original_pos);
	if (err != 0)
		return err;

	unsigned int original_line = original_pos.line;

	struct AlloyCursorPos start_pos;
	start_pos.line = original_line;
	start_pos.column = 1;

	err = alloy_term_set_cursor(term, term_data, &start_pos);
	if (err != 0)
		return err;

	for (unsigned int i = 0; i < width; i++)
		alloy_term_write(term, term_data, " ", 1);

	original_pos.column = 1;

	err = alloy_term_set_cursor(term, term_data, &original_pos);
	if (err != 0)
		return err;

	return 0;
}

int alloy_term_get_char(const struct AlloyTerm *term,
                        struct AlloyTermData *term_data,
                        alloy_utf8 *c)
{
	if (term->get_char == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->get_char(term_data, c);
}

int alloy_term_get_cursor(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data,
                          struct AlloyCursorPos *cursor_pos)
{
	if (term->get_cursor == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->get_cursor(term_data, cursor_pos);
}

int alloy_term_get_size(const struct AlloyTerm *term,
                        struct AlloyTermData *term_data,
                        struct AlloyTermSize *term_size)
{
	if (term->get_size == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->get_size(term_data, term_size);
}

int alloy_term_set_background(const struct AlloyTerm *term,
                              struct AlloyTermData *term_data,
                              const struct AlloyColor *color)
{
	if (term->set_background == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->set_background(term_data, color);
}

int alloy_term_set_cursor(const struct AlloyTerm *term,
                          struct AlloyTermData *term_data,
                          const struct AlloyCursorPos *pos)
{
	if (term->set_cursor == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->set_cursor(term_data, pos);
}

int alloy_term_set_foreground(const struct AlloyTerm *term,
                              struct AlloyTermData *term_data,
                              const struct AlloyColor *color)
{
	if (term->set_foreground == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->set_foreground(term_data, color);
}

int alloy_term_write(const struct AlloyTerm *term,
                     struct AlloyTermData *term_data,
                     const char *str,
                     unsigned int str_len)
{
	if (term->write == ALLOY_NULL)
		return ALLOY_ENOSYS;
	else
		return term->write(term_data, str, str_len);
}

int alloy_term_write_asciiz(const struct AlloyTerm *term,
                            struct AlloyTermData *term_data,
                            const char *str)
{
	while (*str)
	{
		int err = alloy_term_write(term, term_data, str, 1);
		if (err != 0)
			return err;

		str++;
	}

	return 0;
}
