#include <alloy/input.h>

#include <assert.h>
#include <stdlib.h>

char tmp_buffer[16];

static void test_insert(void)
{
	struct AlloyInput alloy_input;

	alloy_input_init(&alloy_input);

	alloy_input.buf = tmp_buffer;
	alloy_input.buf_len = 0;
	alloy_input.buf_res = sizeof(tmp_buffer);
	alloy_input.x_bias = 0;

	/* test back to back insertion */

	int err = alloy_input_insert(&alloy_input, 'a');
	assert(err == 0);
	assert(alloy_input.buf_len == 1);
	assert(alloy_input.buf[0] == 'a');
	assert(alloy_input.buf_pos == 1);
	assert(alloy_input.x_bias == 1);

	err = alloy_input_insert(&alloy_input, 'c');
	assert(err == 0);
	assert(alloy_input.buf_len == 2);
	assert(alloy_input.buf[0] == 'a');
	assert(alloy_input.buf[1] == 'c');
	assert(alloy_input.buf_pos == 2);
	assert(alloy_input.x_bias == 2);

	/* test insertion between two characters */

	alloy_input.buf_pos = 1;

	err = alloy_input_insert(&alloy_input, 'b');
	assert(err == 0);
	assert(alloy_input.buf_len == 3);
	assert(alloy_input.buf[0] == 'a');
	assert(alloy_input.buf[1] == 'b');
	assert(alloy_input.buf[2] == 'c');
	assert(alloy_input.buf_pos == 2);
	/* x_bias should be 2, but that can
	 * be ignored because the change in
	 * position was 'faked' to test the
	 * insertion between characters */
	/* assert(alloy_input.x_bias == 2); */

	/* set the x bias to a correct value
	 * manually */
	alloy_input.x_bias = 2;

	/* test insertion of a newline */
	err = alloy_input_insert(&alloy_input, '\n');
	assert(err == 0);
	assert(alloy_input.buf_len == 4);
	assert(alloy_input.buf[0] == 'a');
	assert(alloy_input.buf[1] == 'b');
	assert(alloy_input.buf[2] == '\n');
	assert(alloy_input.buf[3] == 'c');
	assert(alloy_input.buf_pos == 3);
	assert(alloy_input.x_bias == 0);

	/* test insertion of a tab */

	/* set tab width, so avoid a failing test
	 * because a default value changed */
	alloy_input_set_tab_width(&alloy_input, 4);

	err = alloy_input_insert(&alloy_input, '\t');
	assert(err == 0);
	assert(alloy_input.buf_len == 5);
	assert(alloy_input.buf[0] == 'a');
	assert(alloy_input.buf[1] == 'b');
	assert(alloy_input.buf[2] == '\n');
	assert(alloy_input.buf[3] == '\t');
	assert(alloy_input.buf[4] == 'c');
	assert(alloy_input.buf_pos == 4);
	assert(alloy_input.x_bias == 4);
}

int main(void)
{
	test_insert();
	return EXIT_SUCCESS;
}
