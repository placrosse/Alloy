/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_CMD_H
#define ALLOY_CMD_H

#include <alloy/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** @defgroup cmd-api Command API
 * Deals with the parsing and execution
 * of a command.
 * */

/** Enumerates the builtin commands.
 * @ingroup cmd-api
 * */

enum AlloyCmdID
{
	/** Prints files to the standard output. */
	ALLOY_CMD_CAT,
	/** Changes the working directory. */
	ALLOY_CMD_CD,
	/** Clears the screen. */
	ALLOY_CMD_CLEAR,
	/** Colors a component of the terminal string. */
	ALLOY_CMD_COLOR,
	/** Sets the color scheme of the session. */
	ALLOY_CMD_SCHEME,
	/** List directory contents. */
	ALLOY_CMD_DIR,
	/** Writes to the standard output. */
	ALLOY_CMD_ECHO,
	/** Exits the shell. */
	ALLOY_CMD_EXIT,
	/** Prints help contents. */
	ALLOY_CMD_HELP,
	/** The command was not recognized. */
	ALLOY_CMD_UNKNOWN,
	/** Print the version of Alloy. */
	ALLOY_CMD_VERSION
};

/** Parses a string to identify the command
 * that it contains. The string does not have
 * to be null terminated.
 * identify the command that it contains.
 * @param cmd A pointer to the string to parse.
 * @param cmd_len The number of bytes in the string,
 * not including the null-terminator (if there is one.)
 * @returns The ID of the command.
 * @ingroup cmd-api
 * */

enum AlloyCmdID alloy_cmd_id_parse(const char *cmd, alloy_size cmd_len);

/** Parses a null terminated string to
 * identify the command that it contains.
 * @param cmd A null-terminated command string.
 * @returns The ID of the command.
 * @ingroup cmd-api
 * */

enum AlloyCmdID alloy_cmd_id_parse_z(const char *cmd);

/** Encapsulates data for a command.
 * @ingroup cmd-api
 * */

struct AlloyCmd
{
	/** A pointer to the heap, used for
	 * dynamic memory allocation. */
	struct AlloyHeap *heap;
	/** The command identifier. */
	enum AlloyCmdID id;
	/** The number of command arguments. */
	alloy_size argc;
	/** The command argument array. */
	char **argv;
};

/** Initializes a command structure.
 * @param cmd The command structure to initialize.
 * @ingroup cmd-api
 * */

void alloy_cmd_init(struct AlloyCmd *cmd);

/** Releases memory allocated by the command.
 * @param cmd An initialized command structure.
 * @ingroup cmd-api
 * */

void alloy_cmd_done(struct AlloyCmd *cmd);

/** Assigns the heap to be used for the command
 * data to dynamically grow.
 * @param cmd An initialized command structure.
 * @param heap An initialized heap structure.
 * @ingroup cmd-api
 * */

void alloy_cmd_set_heap(struct AlloyCmd *cmd,
                        struct AlloyHeap *heap);

/** Parses a command from a null-terminated string.
 * @param cmd An initialized command structure.
 * @param line The line to parse the command from.
 * @returns Zero on success, an error code on failure.
 * If the command is unknown, zero is still returned.
 * The id of the command ends up being @ref ALLOY_CMD_UNKNOWN.
 * @ingroup cmd-api
 * */

int alloy_cmd_parse(struct AlloyCmd *cmd, const char *line);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ALLOY_CMD_H */
