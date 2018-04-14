/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_SCANNER_H
#define ALLOY_SCANNER_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup scanner-api Scanner API
 * Implements the scanning of Alloy tokens.
 * */

/** Used to identifier the type
 * of token without looking at the
 * characters.
 * @ingroup scanner-api
 * */

enum AlloyTokenID
{
	/** A null token happens when
	 * there is no string to scan. */
	ALLOY_TOKEN_NULL,
	/** An error token happens when an
	 * unexpected character is found. */
	ALLOY_TOKEN_ERROR,
	/** A newline-delimited comment. */
	ALLOY_TOKEN_COMMENT,
	/** An identifier token. */
	ALLOY_TOKEN_IDENTIFIER,
	/** A numerical token. */
	ALLOY_TOKEN_NUMERICAL,
	/** A string literal token, with either
	 * single quotes or double quotes. */
	ALLOY_TOKEN_STRING,
	/** A single symbol token. */
	ALLOY_TOKEN_SYMBOL,
	/** A space or tab token. */
	ALLOY_TOKEN_SPACE,
	/** A newline token, including
	 * carriage returns and line feeds. */
	ALLOY_TOKEN_NEWLINE
};

/** A token from an Alloy script.
 * This is returned by the lexical
 * scanner in the function @ref alloy_scanner_next.
 * @ingroup scanner-api
 * */

struct AlloyToken
{
	/** The token identifier. */
	enum AlloyTokenID id;
	/** A pointer to the string that
	 * the token started at. */
	const char *buf;
	/** The length of the token buffer. */
	alloy_size buf_len;
};

/** The lexical scanner for Alloy script code.
 * @ingroup scanner-api
 * */

struct AlloyScanner
{
	/** The last token that was scanned. */
	struct AlloyToken last_token;
	/** A pointer to the source code that
	 * the scanner will tokenize. */
	const char *buf;
	/** The length of the source string. */
	alloy_size buf_len;
	/** The position of the scanner within
	 * the source code. */
	alloy_size buf_pos;
	/** The current column of the scanner
	 * within the source code. */
	alloy_size column;
	/** The current line of the scanner
	 * within the source code. */
	alloy_size line;
};

/** Initializes the lexical scanner structure.
 * After calling this function, @ref alloy_scanner_set_buf
 * should be called.
 * @param scanner The scanner structure to initialize.
 * @ingroup scanner-api
 * */

void alloy_scanner_init(struct AlloyScanner *scanner);

/** Sets the string buffer to be scanned.
 * @param scanner An initialized scanner structure.
 * @param buf The string to tokenize.
 * @param buf_len The number of bytes in the string.
 * This does not include the null terminating character,
 * if there is one.
 * @ingroup scanner-api
 * */

void alloy_scanner_set_buf(struct AlloyScanner *scanner,
                           const char *buf,
                           alloy_size buf_len);

/** Resets the scanner to the beginning of the string buffer.
 * @param scanner An initialized scanner structure.
 * @ingroup scanner-api
 * */

void alloy_scanner_begin(struct AlloyScanner *scanner);

/** Scans the next token in the source code.
 * The function @ref alloy_scanner_set_buf should be
 * called before using this function.
 * @param scanner An initialized scanner structure.
 * @returns The token that was found by this function.
 * If the scanner is already at the end of the source,
 * then @ref ALLOY_NULL is returned.
 * @ingroup scanner-api
 * */

struct AlloyToken *alloy_scanner_next(struct AlloyScanner *scanner);

/** Checks whether or not the scanner has reached the
 * end of the source code.
 * @param scanner An initialized scanner structure.
 * @returns If the end of the source code has been
 * reached, @ref ALLOY_TRUE is returned. Otherwise,
 * @ref ALLOY_FALSE is returned.
 * @ingroup scanner-api
 * */

alloy_bool alloy_scanner_eof(const struct AlloyScanner *scanner);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_SCANNER_H */
