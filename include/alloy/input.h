#ifndef ALLOY_INPUT_H
#define ALLOY_INPUT_H

/** Used for processing input keys from
 * the user.
 * */

struct AlloyInput
{
	/** The address to the block of memory
	 * that will contain the input characters. */
	char *buf;
	/** The number of characters in the buffer. */
	unsigned int buf_len;
	/** The maximum number of characters allowed
	 * in the buffer. */
	unsigned int buf_res;
	/** The current position of the cursor in the
	 * input buffer. */
	unsigned int buf_pos;
	/** The column that should be moved to, if possible,
	 * whenever an up or down key is pressed. */
	unsigned int x_bias;
	/** The number of spaces that make up a tab. */
	unsigned int tab_width;
};

void alloy_input_init(struct AlloyInput *input);

void alloy_input_set_tab_width(struct AlloyInput *input,
                               unsigned int tab_size);

int alloy_input_insert(struct AlloyInput *input, char c);

int alloy_input_backspace(struct AlloyInput *input);

int alloy_input_delete(struct AlloyInput *input);

int alloy_input_up(struct AlloyInput *input);

int alloy_input_down(struct AlloyInput *input);

int alloy_input_left(struct AlloyInput *input);

int alloy_input_right(struct AlloyInput *input);

#endif /* ALLOY_INPUT_H */
