/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_HISTORY_H
#define ALLOY_HISTORY_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;

/** @defgroup history-api History API
 * Used to retrieve and add to command line
 * history.
 * */

/** This is the default maximum number of
 * input lines to store in memory. The maximum
 * is there to prevent a large accumulation of lines
 * in memory, that could otherwise be used in
 * a more useful way.
 * @ingroup history-api
 * */

#define ALLOY_HISTORY_DEFAULT_MAX 100

/** Contains the input lines previously
 * entered by the user. This offers a useful
 * mechanism in which the user does not have
 * to enter a command previously entered over again.
 * @ingroup history-api
 * */

struct AlloyHistory
{
	/** The heap to allocate the history on. */
	struct AlloyHeap *heap;
	/** The array of input lines. */
	char **lines;
	/** The number of input lines. */
	alloy_size line_count;
	/** The current input line. */
	alloy_size line_index;
	/** The maximum number of lines to store.
	 * The default value is @ref ALLOY_HISTORY_DEFAULT_MAX */
	alloy_size line_max;
};

/** Initializes the history structure.
 * @param history The history structure to initialize.
 * @param heap The heap that will be used to store history lines.
 * */

void alloy_history_init(struct AlloyHistory *history,
                        struct AlloyHeap *heap);

/** Releases all the memory allocated by
 * the history structure.
 * @param history An initialized history structure.
 * @ingroup history-api
 * */

void alloy_history_done(struct AlloyHistory *history);

/** Adds a new line to the history structure.
 * Despite the current line index, this new line
 * is added to the end of the history list.
 * @param history An initialized history structure.
 * @param line The line to add to the history structure.
 * @returns Zero on success, an error code on failure.
 * @ingroup history-api
 * */

int alloy_history_put(struct AlloyHistory *history,
                      const char *line);

/** Go down in the history list. Going down in the
 * history list means going to an older history line.
 * @param history An initialized history structure.
 * @returns The command line previous to the current one.
 * If there are no command lines, then @ref ALLOY_NULL is
 * returned. If the history is already at the oldest line,
 * then @ref ALLOY_NULL is returned.
 * @ingroup history-api
 * */

const char *alloy_history_down(struct AlloyHistory *history);

/** Go up in the history list. Going up in the
 * history list means going to a newer history line.
 * @param history An initialized history structure.
 * @returns The newer history line, compared to the current.
 * If the history is already at the newest line, then @ref ALLOY_NULL
 * is returned instead.
 * @ingroup history-api
 * */

const char *alloy_history_up(struct AlloyHistory *history);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_HISTORY_H */
