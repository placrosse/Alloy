/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_INPUT_H
#define ALLOY_INPUT_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** @defgroup input-api Input API
 * Used for gathering textual input from the user.
 * */

/** Contains an editable line of text
 * that is modified by the user.
 * @ingroup input-api
 * */

struct AlloyInput
{
	/** A pointer to the heap used for
	 * managing the input buffer. */
	struct AlloyHeap *heap;
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
 * called before any other input function.
 * @param input The input structure to initialize.
 * @ingroup input-api
 * */

void alloy_input_init(struct AlloyInput *input);

/** Releases memory allocated by the input structure.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

void alloy_input_done(struct AlloyInput *input);

/** Assigns the heap to be used by the input structure
 * to manage the input buffer.
 * @param input An initialized input structure.
 * @param heap An initialized heap structure.
 * @ingroup input-api
 * */

void alloy_input_set_heap(struct AlloyInput *input,
                          struct AlloyHeap *heap);

/** Clears the contents of the input string.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

void alloy_input_clear(struct AlloyInput *input);

/** Sets the tab width.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

void alloy_input_set_tab_width(struct AlloyInput *input,
                               unsigned int tab_size);

/** Inserts a character at the cursor position.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_insert(struct AlloyInput *input, char c);

/** Deletes the character directly behind the cursor
 * and then moves the cursor left.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_backspace(struct AlloyInput *input);

/** Deletes the character that the cursor is
 * currently pointing at.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_delete(struct AlloyInput *input);

/** Moves the cursor up one line.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_up(struct AlloyInput *input);

/** Moves the cursor down one line.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_down(struct AlloyInput *input);

/** Moves the cursor to the left.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_left(struct AlloyInput *input);

/** Reserve an amount of memory for the input
 * buffer to grow. This is done automatically,
 * but calling this function directly is good
 * for performance when the size that the input
 * buffer will grow to is known.
 * @param input An initialized input structure.
 * @param res The number of bytes to reserve for
 * the input buffer to grow.
 * @returns Zero on success, an error code on failure.
 * @ingroup input-api
 * */

int alloy_input_reserve(struct AlloyInput *input,
                        alloy_size res);

/** Moves the cursor to the right.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_right(struct AlloyInput *input);

/** Moves the cursor to the beginning of the line.
 * @param input An initialized input structure.
 * @ingroup input-api
 * */

int alloy_input_home(struct AlloyInput *input);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_INPUT_H */
