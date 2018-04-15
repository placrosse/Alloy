/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/scanner.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Test
{
	enum AlloyTokenID id;
	const char *buf;
};

int test_run(struct AlloyScanner *scanner, const struct Test *test)
{
	struct AlloyToken *token = alloy_scanner_next(scanner);
	if (token == ALLOY_NULL)
	{
		fprintf(stderr, "Token was null.\n");
		return EXIT_FAILURE;
	}

	if (token->id != test->id)
	{
		fprintf(stderr, "Expected token '%d', got '%d'.\n",
		        (int) test->id, (int) token->id);
		return EXIT_FAILURE;
	}

	if (token->buf_len != strlen(test->buf))
	{
		fprintf(stderr, "Expected token length : %u\n", (unsigned int) strlen(test->buf));
		fprintf(stderr, "Actual token length   : %u\n", (unsigned int) token->buf_len);
		return EXIT_FAILURE;
	}

	if (memcmp(token->buf, token->buf, token->buf_len) != 0)
	{
		fprintf(stderr, "Expected token : %s\n", test->buf);
		fprintf(stderr, "Actual token   : %.*s\n", (int) token->buf_len, token->buf);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(void)
{
	const char src[] = "# This is a comment.\n"
	                   "_id  0x00\t >_id2\r\n"
	                   "\'ab\' \"cd\"";

	struct AlloyScanner scanner;
	alloy_scanner_init(&scanner);
	alloy_scanner_set_buf(&scanner, src, sizeof(src) - 1);
	alloy_scanner_begin(&scanner);

	struct Test test1;
	test1.id = ALLOY_TOKEN_COMMENT;
	test1.buf = "# This is a comment.";
	assert(test_run(&scanner, &test1) == EXIT_SUCCESS);

	struct Test test2;
	test2.id = ALLOY_TOKEN_NEWLINE;
	test2.buf = "\n";
	assert(test_run(&scanner, &test2) == EXIT_SUCCESS);

	struct Test test3;
	test3.id = ALLOY_TOKEN_IDENTIFIER;
	test3.buf = "_id";
	assert(test_run(&scanner, &test3) == EXIT_SUCCESS);

	struct Test test4;
	test4.id = ALLOY_TOKEN_SPACE;
	test4.buf = "  ";
	assert(test_run(&scanner, &test4) == EXIT_SUCCESS);

	struct Test test5;
	test5.id = ALLOY_TOKEN_NUMERICAL;
	test5.buf = "0x00";
	assert(test_run(&scanner, &test5) == EXIT_SUCCESS);

	struct Test test6;
	test6.id = ALLOY_TOKEN_SPACE;
	test6.buf = "\t ";
	assert(test_run(&scanner, &test6) == EXIT_SUCCESS);

	struct Test test7;
	test7.id = ALLOY_TOKEN_SYMBOL;
	test7.buf = ">";
	assert(test_run(&scanner, &test7) == EXIT_SUCCESS);

	struct Test test8;
	test8.id = ALLOY_TOKEN_IDENTIFIER;
	test8.buf = "_id2";
	assert(test_run(&scanner, &test8) == EXIT_SUCCESS);

	struct Test test9;
	test9.id = ALLOY_TOKEN_NEWLINE;
	test9.buf = "\r\n";
	assert(test_run(&scanner, &test9) == EXIT_SUCCESS);

	struct Test test10;
	test10.id = ALLOY_TOKEN_STRING;
	test10.buf = "\'ab\'";
	assert(test_run(&scanner, &test10) == EXIT_SUCCESS);

	struct Test test11;
	test11.id = ALLOY_TOKEN_SPACE;
	test11.buf = " ";
	assert(test_run(&scanner, &test11) == EXIT_SUCCESS);

	struct Test test12;
	test12.id = ALLOY_TOKEN_STRING;
	test12.buf = "\"cd\"";
	assert(test_run(&scanner, &test12) == EXIT_SUCCESS);

	assert(alloy_scanner_eof(&scanner));

	return EXIT_SUCCESS;
}
