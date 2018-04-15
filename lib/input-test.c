#include <alloy/input.h>

#include <alloy/heap.h>

#include <assert.h>
#include <stdlib.h>

unsigned char buf[128];

static void test_insert(void)
{
	struct AlloyHeap heap;
	alloy_heap_init(&heap, buf, sizeof(buf));

	struct AlloyInput input;
	alloy_input_init(&input);
	alloy_input_set_heap(&input, &heap);

	/* test back to back insertion */

	int err = alloy_input_insert(&input, 'a');
	assert(err == 0);
	assert(input.buf_len == 1);
	assert(input.buf[0] == 'a');
	assert(input.buf_pos == 1);
	assert(input.x_bias == 1);

	err = alloy_input_insert(&input, 'c');
	assert(err == 0);
	assert(input.buf_len == 2);
	assert(input.buf[0] == 'a');
	assert(input.buf[1] == 'c');
	assert(input.buf_pos == 2);
	assert(input.x_bias == 2);

	/* test insertion between two characters */

	input.buf_pos = 1;

	err = alloy_input_insert(&input, 'b');
	assert(err == 0);
	assert(input.buf_len == 3);
	assert(input.buf[0] == 'a');
	assert(input.buf[1] == 'b');
	assert(input.buf[2] == 'c');
	assert(input.buf_pos == 2);
	/* x_bias should be 2, but that can
	 * be ignored because the change in
	 * position was 'faked' to test the
	 * insertion between characters */
	/* assert(input.x_bias == 2); */

	/* set the x bias to a correct value
	 * manually */
	input.x_bias = 2;

	/* test insertion of a newline */
	err = alloy_input_insert(&input, '\n');
	assert(err == 0);
	assert(input.buf_len == 4);
	assert(input.buf[0] == 'a');
	assert(input.buf[1] == 'b');
	assert(input.buf[2] == '\n');
	assert(input.buf[3] == 'c');
	assert(input.buf_pos == 3);
	assert(input.x_bias == 0);

	/* test insertion of a tab */

	/* set tab width, so avoid a failing test
	 * because a default value changed */
	alloy_input_set_tab_width(&input, 4);

	err = alloy_input_insert(&input, '\t');
	assert(err == 0);
	assert(input.buf_len == 5);
	assert(input.buf[0] == 'a');
	assert(input.buf[1] == 'b');
	assert(input.buf[2] == '\n');
	assert(input.buf[3] == '\t');
	assert(input.buf[4] == 'c');
	assert(input.buf_pos == 4);
	assert(input.x_bias == 4);

	alloy_input_done(&input);
}

static void test_movement(void)
{
	struct AlloyHeap heap;
	alloy_heap_init(&heap, buf, sizeof(buf));

	struct AlloyInput input;
	alloy_input_init(&input);
	alloy_input_set_heap(&input, &heap);

	/* check for left movement */

	int err = alloy_input_reserve(&input, 5);
	assert(err == 0);

	input.buf[0] = 'a';
	input.buf[1] = '\n';
	input.buf[2] = 'b';
	input.buf[3] = 'c';
	input.buf[4] = 'd';
	input.buf_len = 5;
	input.buf_pos = 5;

	/* x bias could be anything at this point.
	 * it should be adjusting to the current
	 * line on left and right arrows */
	input.x_bias = 55;

	err = alloy_input_left(&input);
	assert(err == 0);
	assert(input.buf_pos == 4);
	assert(input.x_bias == 2);

	err |= alloy_input_left(&input);
	err |= alloy_input_left(&input);
	err |= alloy_input_left(&input);
	assert(err == 0);
	assert(input.buf_pos == 1);
	assert(input.x_bias == 1);

	err = alloy_input_right(&input);
	assert(err == 0);
	assert(input.buf_pos == 2);
	assert(input.x_bias == 0);

	alloy_input_done(&input);
}

static void test_backspace(void)
{

}

static void test_delete(void)
{

}

int main(void)
{
	test_insert();
	test_movement();
	test_backspace();
	test_delete();
	return EXIT_SUCCESS;
}
