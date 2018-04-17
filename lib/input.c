#include <alloy/input.h>

#include <alloy/errno.h>
#include <alloy/heap.h>

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
	input->heap = ALLOY_NULL;
	input->buf = ALLOY_NULL;
	input->buf_len = 0;
	input->buf_pos = 0;
	input->buf_res = 0;
	input->x_bias = 0;
}

void alloy_input_done(struct AlloyInput *input)
{
	if (input->heap == ALLOY_NULL)
		return;

	alloy_heap_free(input->heap, input->buf);

	input->buf = ALLOY_NULL;
	input->buf_len = 0;
	input->buf_pos = 0;
	input->buf_res = 0;
	input->x_bias = 0;
}

void alloy_input_set_heap(struct AlloyInput *input,
                          struct AlloyHeap *heap)
{
	/* Make sure that the current heap, if present,
	 * does not get overwritten. This would mess up
	 * reallocations of the input buffer. */
	if (input->heap != ALLOY_NULL)
		return;

	/* Make sure a heap was actually passed. */
	if (heap == ALLOY_NULL)
		return;

	input->heap = heap;
}

void alloy_input_clear(struct AlloyInput *input)
{
	for (unsigned int i = 0; i < input->buf_res; i++)
		input->buf[i] = 0;

	input->buf_pos = 0;
	input->buf_len = 0;
	input->x_bias = 0;
}

void alloy_input_set_tab_width(struct AlloyInput *input,
                               unsigned int tab_width)
{
	input->tab_width = tab_width;
}

int alloy_input_reserve(struct AlloyInput *input,
                        alloy_size res)
{
	if (input->heap == ALLOY_NULL)
		return ALLOY_EFAULT;

	char *buf = input->buf;

	buf = alloy_heap_realloc(input->heap, buf, res);
	if (buf == ALLOY_NULL)
		return ALLOY_ENOMEM;

	input->buf = buf;
	input->buf_res = res;

	if (input->buf_len > res)
		input->buf_len = res;

	if (input->buf_pos > res)
		input->buf_pos = res;

	return 0;
}

int alloy_input_backspace(struct AlloyInput *input)
{
	if (input->buf_pos <= 0)
		return 0;

	input->buf_pos--;

	for (unsigned int i = input->buf_pos; i < input->buf_len; i++)
		input->buf[i] = input->buf[i + 1];

	input->buf[input->buf_len - 1] = 0;

	input->buf_len--;

	return calculate_x_bias(input);
}

int alloy_input_insert(struct AlloyInput *input, char c)
{
	if ((input->buf_pos + 1) >= input->buf_res)
	{
		alloy_size new_size = ((input->buf_pos + 63) / 64) * 64;

		int err = alloy_input_reserve(input, new_size);
		if (err != 0)
			return err;
	}

	if ((c < ' ') && (c > '~') && (c != '\t'))
		return ALLOY_EINVAL;

	for (unsigned int i = input->buf_len; i > input->buf_pos; i--)
		input->buf[i] = input->buf[i - 1];

	input->buf[input->buf_pos++] = c;

	input->buf_len++;

	/* Add the null-terminator, if the
	 * character was added to the end of
	 * the string. */
	if (input->buf_pos >= input->buf_len)
		input->buf[input->buf_pos] = 0;

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
		return 0;

	input->buf_pos--;

	return calculate_x_bias(input);
}

int alloy_input_right(struct AlloyInput *input)
{
	if (input->buf_pos >= input->buf_len)
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
