#include <alloy/term.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void alloy_term_init(struct AlloyTerm *term)
{
	term->data = NULL;
	term->done = NULL;
	term->clear = NULL;
	term->get_size = NULL;
	term->toggle_cursor = NULL;
	term->set_background = NULL;
	term->set_cursor = NULL;
	term->set_foreground = NULL;
	term->write = NULL;
}

void alloy_term_done(struct AlloyTerm *term)
{
	if (term->done != NULL)
		term->done(term->data);
}

int alloy_term_clear(struct AlloyTerm *term)
{
	if (term->clear == NULL)
		return -EFAULT;

	return term->clear(term->data);
}

int alloy_term_get_size(struct AlloyTerm *term,
                        unsigned int *width,
                        unsigned int *height)
{
	if (term->get_size == NULL)
		return -1;

	return term->get_size(term->data, width, height);
}

int alloy_term_toggle_cursor(struct AlloyTerm *term)
{
	if (term->toggle_cursor == NULL)
		return -EFAULT;

	return term->toggle_cursor(term->data);
}

int alloy_term_set_background(struct AlloyTerm *term,
                              unsigned long int color)
{
	if (term->set_background == NULL)
		return -EFAULT;

	return term->set_background(term->data, color);
}

int alloy_term_set_cursor(struct AlloyTerm *term,
                          unsigned int line,
                          unsigned int column)
{
	if (term->set_cursor == NULL)
		return -EFAULT;

	return term->set_cursor(term->data, line, column);
}

int alloy_term_set_foreground(struct AlloyTerm *term,
                              unsigned long int color)
{
	if (term->set_foreground == NULL)
		return -EFAULT;

	return term->set_foreground(term->data, color);
}

int alloy_term_write(struct AlloyTerm *term,
                     const char *str,
                     unsigned int str_len)
{
	if (term->write != NULL)
		term->write(term->data, str, str_len);

	return 0;
}

int alloy_term_write_asciiz(struct AlloyTerm *term,
                            const char *str)
{
	unsigned int str_len = strlen(str);

	return alloy_term_write(term, str, str_len);
}
