#include <alloy/input.h>

#include <alloy/error.h>

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

void alloy_input_init(struct AlloyInput *input)
{
	input->buf = NULL;
	input->buf_len = 0;
	input->buf_pos = 0;
	input->x_bias = 0;
}

void alloy_input_set_tab_width(struct AlloyInput *input,
                               unsigned int tab_width)
{
	input->tab_width = tab_width;
}

int alloy_input_insert(struct AlloyInput *input, char c)
{
	if (input->buf_pos >= input->buf_res)
		return ALLOY_ERROR_BUFFER_FULL;
	else if ((c < ' ') && (c > '~') && (c != '\t'))
		return ALLOY_ERROR_UNKNOWN_CHARACTER;


	if (c == '\t')
	{

	}

	for (unsigned int i = input->buf_len; i > input->buf_pos; i--)
		input->buf[i] = input->buf[i - 1];

	input->buf[input->buf_pos++] = c;
	input->buf_len++;

	if (c == '\n')
		input->x_bias = 0;
	else if (c == '\t')
		input->x_bias += input->tab_width - (input->x_bias % input->tab_width);
	else
		input->x_bias++;
	
	return 0;
}
