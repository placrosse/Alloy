#include <alloy/input.h>

#include <alloy/error.h>

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

static int calculate_x_bias(struct AlloyInput *input)
{
	unsigned int i = input->buf_pos;

	/* rewind to the beginning of the line */

	while (i > 0)
	{
		if (input->buf[i - 1] == '\n')
			break;
		i--;
	}

	/* iterate back to the buffer position,
	 * calculating the x bias */

	unsigned int x_bias = 0;

	while (i < input->buf_pos)
	{
		if (input->buf[i] == '\t')
			x_bias += input->tab_width - (x_bias % input->tab_width);
		else
			x_bias++;
		i++;
	}

	input->x_bias = x_bias;

	return 0;
}

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

int alloy_input_left(struct AlloyInput *input)
{
	if (input->buf_pos <= 0)
		/* TODO : Should this error? */
		return 0;

	input->buf_pos--;

	return calculate_x_bias(input);
}

int alloy_input_right(struct AlloyInput *input)
{
	if (input->buf_pos >= input->buf_len)
		/* TODO : Should this error? */
		return 0;

	input->buf_pos++;

	/* determine the new x bias */

	return calculate_x_bias(input);
}

int alloy_input_home(struct AlloyInput *input)
{
	(void) input;
	return 0;
}
