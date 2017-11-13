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

/** Initializes the structure. This should be
 * called before any other input function. */

void alloy_input_init(struct AlloyInput *input);

/** Clears the contents of the input string. */

void alloy_input_clear(struct AlloyInput *input);

/** Sets the tab width. */

void alloy_input_set_tab_width(struct AlloyInput *input,
                               unsigned int tab_size);

/** Inserts a character at the cursor position. */

int alloy_input_insert(struct AlloyInput *input, char c);

/** Deletes the character directly behind the cursor
 * and then moves the cursor left. */

int alloy_input_backspace(struct AlloyInput *input);

/** Deletes the character that the cursor is
 * currently pointing at. */

int alloy_input_delete(struct AlloyInput *input);

/** Moves the cursor up one line. */

int alloy_input_up(struct AlloyInput *input);

/** Moves the cursor down one line. */

int alloy_input_down(struct AlloyInput *input);

/** Moves the cursor to the left. */

int alloy_input_left(struct AlloyInput *input);

/** Moves the cursor to the right. */

int alloy_input_right(struct AlloyInput *input);

/** Moves the cursor to the beginning of the line. */

int alloy_input_home(struct AlloyInput *input);

#endif /* ALLOY_INPUT_H */
