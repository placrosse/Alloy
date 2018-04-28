/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_ALLOY_H
#define ALLOY_ALLOY_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHeap;
struct AlloyHost;
struct AlloyHostData;
struct AlloyShell;
struct AlloyTerm;
struct AlloyTermData;

/** @defgroup alloy-api Alloy API
 * The high level interface for using the library.
 * */

/** This structure contains the shell, terminal,
 * and host implementation. It is used to contain
 * the system as a whole.
 * @ingroup alloy-api
 * */

struct Alloy
{
	/** The heap used for allocating memory in
	 * the other structures. */
	struct AlloyHeap *heap;
	/** An initialized host implementation. */
	const struct AlloyHost *host;
	/** Contains host implementation data. */
	struct AlloyHostData *host_data;
	/** The shell used to execute commands. */
	struct AlloyShell *shell;
	/** The terminal used to display the output. */
	const struct AlloyTerm *term;
	/** The terminal implementation data. */
	struct AlloyTermData *term_data;
};

/** Initializes the system.
 * @param alloy An uninitialized alloy structure.
 * @ingroup alloy-api
 * */

void alloy_init(struct Alloy *alloy);

/** Releases memory allocated by the system.
 * @param alloy An initialized alloy structure.
 * @ingroup alloy-api
 */

void alloy_done(struct Alloy *alloy);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_ALLOY_H */
