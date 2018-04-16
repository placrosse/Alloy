/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/scanner.h>

static alloy_bool is_alpha(char c)
{
	if ((c >= 'a') && (c <= 'z'))
		return ALLOY_TRUE;
	else if ((c >= 'A') && (c <= 'Z'))
		return ALLOY_TRUE;
	else
		return ALLOY_FALSE;
}

static alloy_bool is_numerical(char c)
{
	if ((c >= '0') && (c <= '9'))
		return ALLOY_TRUE;
	else
		return ALLOY_FALSE;
}

static alloy_bool is_identifier(char c)
{
	if (is_alpha(c) || is_numerical(c) || (c == '_'))
		return ALLOY_TRUE;
	else
		return ALLOY_FALSE;
}

static struct AlloyToken *comment(struct AlloyScanner *scanner)
{
	const char *buf = &scanner->buf[scanner->buf_pos];
	if (buf[0] != '#')
		return ALLOY_NULL;

	scanner->last_token.id = ALLOY_TOKEN_COMMENT;
	scanner->last_token.buf = buf;

	alloy_size i = 0;
	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	while (i < buf_len)
	{
		if ((buf[i] == '\n')
		 || (buf[i] == '\r'))
			break;

		i++;
	}

	scanner->last_token.buf_len = i;

	scanner->buf_pos += i;

	return &scanner->last_token;
}

static struct AlloyToken *newline(struct AlloyScanner *scanner)
{
	const char *buf = &scanner->buf[scanner->buf_pos];

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	struct AlloyToken *token = &scanner->last_token;

	if (buf_len >= 2)
	{
		if ((buf[0] == '\r')
		 && (buf[1] == '\n'))
		{
			token->buf = buf;
			token->buf_len = 2;
			token->id = ALLOY_TOKEN_NEWLINE;
			scanner->buf_pos += 2;
			return token;
		}

		if ((buf[0] == '\n')
		 && (buf[1] == '\r'))
		{
			token->buf = buf;
			token->buf_len = 2;
			token->id = ALLOY_TOKEN_NEWLINE;
			scanner->buf_pos += 2;
			return token;
		}
	}

	if ((buf[0] == '\n')
	 || (buf[0] == '\r'))
	{
		token->buf = buf;
		token->buf_len = 1;
		token->id = ALLOY_TOKEN_NEWLINE;
		scanner->buf_pos++;
		return token;
	}

	return ALLOY_NULL;
}

static struct AlloyToken *identifier(struct AlloyScanner *scanner)
{
	const char *buf = &scanner->buf[scanner->buf_pos];

	/* First character must be a letter or '_' */
	if (!is_alpha(buf[0]) && (buf[0] != '_'))
		return ALLOY_NULL;

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	alloy_size i = 0;

	scanner->last_token.id = ALLOY_TOKEN_IDENTIFIER;
	scanner->last_token.buf = buf;

	while ((i < buf_len) && is_identifier(buf[i]))
	{
		i++;
	}

	scanner->last_token.buf_len = i;

	scanner->buf_pos += i;

	return &scanner->last_token;
}

static struct AlloyToken *numerical(struct AlloyScanner *scanner)
{
	const char *buf = &scanner->buf[scanner->buf_pos];

	if (!is_numerical(buf[0]))
		return ALLOY_NULL;

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	alloy_size i = 0;

	scanner->last_token.id = ALLOY_TOKEN_NUMERICAL;
	scanner->last_token.buf = buf;

	while ((i < buf_len) && is_identifier(buf[i]))
	{
		i++;
	}

	scanner->last_token.buf_len = i;

	scanner->buf_pos += i;

	return &scanner->last_token;
}

static struct AlloyToken *space(struct AlloyScanner *scanner)
{
	const char *buf = &scanner->buf[scanner->buf_pos];

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	alloy_size i = 0;

	while (i < buf_len)
	{
		if ((buf[i] != ' ')
		 && (buf[i] != '\t'))
			break;

		i++;
	}

	if (i == 0)
		return ALLOY_NULL;

	scanner->last_token.id = ALLOY_TOKEN_SPACE;
	scanner->last_token.buf = buf;
	scanner->last_token.buf_len = i;
	scanner->buf_pos += i;

	return &scanner->last_token;
}

static struct AlloyToken *symbol(struct AlloyScanner *scanner)
{
	struct AlloyToken *token = &scanner->last_token;

	const char *buf = &scanner->buf[scanner->buf_pos];

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;

	token->id = ALLOY_TOKEN_SYMBOL;
	token->buf = buf;

	if (buf_len > 2)
	{
		if ((buf[0] == '|')
		 && (buf[1] == '|'))
		{
			token->buf_len = 2;
			scanner->buf_pos += 2;
			return token;
		}

		if ((buf[0] == '&')
		 && (buf[1] == '&'))
		{
			token->buf_len = 2;
			scanner->buf_pos += 2;
			return token;
		}
	}

	if ((buf[0] == '=')
	 || (buf[0] == '+')
	 || (buf[0] == '-')
	 || (buf[0] == '<')
	 || (buf[0] == '>')
	 || (buf[0] == '{')
	 || (buf[0] == '}')
	 || (buf[0] == '(')
	 || (buf[0] == ')')
	 || (buf[0] == '[')
	 || (buf[0] == ']')
	 || (buf[0] == '*')
	 || (buf[0] == '&')
	 || (buf[0] == '^')
	 || (buf[0] == '%')
	 || (buf[0] == '$')
	 || (buf[0] == '@')
	 || (buf[0] == '!')
	 || (buf[0] == '~')
	 || (buf[0] == '`')
	 || (buf[0] == ';')
	 || (buf[0] == ':')
	 || (buf[0] == ',')
	 || (buf[0] == '.')
	 || (buf[0] == '/')
	 || (buf[0] == '\\')
	 || (buf[0] == '?')
	 || (buf[0] == '|'))
	{
		token->buf_len = 1;
		scanner->buf_pos++;
		return token;
	}

	return ALLOY_NULL;
}

static struct AlloyToken *string(struct AlloyScanner *scanner)
{
	char terminator = 0;

	const char *buf = &scanner->buf[scanner->buf_pos];
	if (buf[0] == '\"')
		terminator = '\"';
	else if (buf[0] == '\'')
		terminator = '\'';
	else
		return ALLOY_NULL;

	alloy_size buf_len = scanner->buf_len - scanner->buf_pos;
	alloy_size i = 1;

	while (i < buf_len)
	{
		if (buf[i] == terminator)
			break;

		i++;
	}

	if (i >= buf_len)
		return ALLOY_NULL;

	struct AlloyToken *token = &scanner->last_token;
	token->id = ALLOY_TOKEN_STRING;
	token->buf = buf;
	token->buf_len = i + 1;

	scanner->buf_pos += i + 1;

	return token;
}

static struct AlloyToken *error(struct AlloyScanner *scanner)
{
	scanner->last_token.id = ALLOY_TOKEN_ERROR;
	scanner->last_token.buf = &scanner->buf[scanner->buf_pos];
	scanner->last_token.buf_len = 1;
	scanner->buf_pos++;
	return &scanner->last_token;
}

void alloy_scanner_init(struct AlloyScanner *scanner)
{
	scanner->buf = ALLOY_NULL;
	scanner->buf_pos = 0;
	scanner->buf_len = 0;
	scanner->line = 1;
	scanner->column = 1;
	scanner->ignore_space = ALLOY_FALSE;
}

void alloy_scanner_ignore_space(struct AlloyScanner *scanner)
{
	scanner->ignore_space = ALLOY_TRUE;
}

void alloy_scanner_begin(struct AlloyScanner *scanner)
{
	scanner->buf_pos = 0;
	scanner->line = 1;
	scanner->column = 1;
}

void alloy_scanner_set_buf(struct AlloyScanner *scanner,
                           const char *buf,
                           alloy_size buf_len)
{
	scanner->buf = buf;
	scanner->buf_len = buf_len;
	scanner->buf_pos = 0;
	scanner->line = 1;
	scanner->column = 1;
}

void alloy_scanner_set_buf_z(struct AlloyScanner *scanner,
                             const char *buf_z)
{
	alloy_size i = 0;

	while (buf_z[i] != 0)
		i++;

	alloy_scanner_set_buf(scanner, buf_z, i);
}

struct AlloyToken *alloy_scanner_next(struct AlloyScanner *scanner)
{
	if (alloy_scanner_eof(scanner))
		return ALLOY_NULL;

	struct AlloyToken *token = identifier(scanner);
	if (token != ALLOY_NULL)
		return token;

	token = numerical(scanner);
	if (token != ALLOY_NULL)
		return token;

	token = comment(scanner);
	if (token != ALLOY_NULL)
		return token;

	token = newline(scanner);
	if (token != ALLOY_NULL)
		return token;

	token = space(scanner);
	if (token != ALLOY_NULL)
	{
		if (scanner->ignore_space)
			return alloy_scanner_next(scanner);
		else
			return token;
	}

	token = symbol(scanner);
	if (token != ALLOY_NULL)
		return token;

	token = string(scanner);
	if (token != ALLOY_NULL)
		return token;

	return error(scanner);
}

alloy_bool alloy_scanner_eof(const struct AlloyScanner *scanner)
{
	if (scanner->buf_pos >= scanner->buf_len)
		return ALLOY_TRUE;
	else
		return ALLOY_FALSE;
}
