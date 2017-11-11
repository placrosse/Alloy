#include <alloy/term.h>

#include <errno.h>
#include <stdlib.h>

void alloy_term_init(struct AlloyTerm *term)
{
	term->data = NULL;
	term->done = NULL;
	term->write = NULL;
}

void alloy_term_done(struct AlloyTerm *term)
{
	if (term->done != NULL)
		term->done(term->data);
}

int alloy_term_write(struct AlloyTerm *term,
                     const char *str,
                     unsigned int str_len)
{
	if (term->write != NULL)
		term->write(term->data, str, str_len);

	return 0;
}
