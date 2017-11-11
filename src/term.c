#include <alloy/term.h>

#include <errno.h>
#include <stdlib.h>

void alloy_term_init(struct AlloyTerm *term)
{
	term->data = NULL;
	term->done = NULL;
	term->set_foreground = NULL;
	term->set_background = NULL;
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

int alloy_term_set_background(struct AlloyTerm *term,
                              unsigned long int color)
{
	if (term->set_background == NULL)
		return -EFAULT;

	return term->set_background(term->data, color);
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
