/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#ifndef ALLOY_APP_H
#define ALLOY_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyHost;
struct AlloyHost;
struct AlloyTerm;
struct AlloyTermData;

/** @defgroup app-api Application API
 * Used by Alloy to start applications.
 * */

/** Contains data required by the API
 * to call system functions.
 * @ingroup app-api
 * */

struct AlloyAppHost
{
	/** An initialized terminal structure. */
	const struct AlloyTerm *term;
	/** Terminal implementation data. */
	struct AlloyTermData *term_data;
	/** An initialized host structure. */
	const struct AlloyHost *host;
	/** Host implementation data. */
	struct AlloyHostData *host_data;
};

/** Contains the callbacks and data used to
 * implement the API. This structure is passed
 * as a parameter to the application.
 * @ingroup app-api
 * */

struct AlloyAppContainer
{
	/** Points to the data that implements the API. */
	struct AlloyAppHost *host;
	/** Number of arguments to the app. */
	int argc;
	/** The arguments to the app. */
	char **argv;
	/** Environment variables for app. Currently,
	 * this is set to @ref ALLOY_NULL. */
	char **env;
	/** Writes to a file descriptor. */
	int (*write)(int fd, const void *buf, unsigned int len, struct AlloyAppHost *app_host);
	/** Reads from a file descriptor. */
	int (*read)(int fd, void *buf, unsigned int len, struct AlloyAppHost *app_host);
	/** Opens a file. */
	int (*open)(const char *path, int mode, struct AlloyAppHost *app_host);
	/** Closes a file. */
	int (*close)(int fd, struct AlloyAppHost *app_host);
	/** Allocates memory. */
	void *(*malloc)(unsigned long int size, struct AlloyAppHost *app_host);
	/** Re-allocates memory. */
	void *(*realloc)(void *addr, unsigned long int size, struct AlloyAppHost *app_host);
	/** Releases memory to be reused. */
	void (*free)(void *addr, struct AlloyAppHost *app_host);
};

/** Contains callbacks and data used to implement
 * the API, as well as other meta-data of the application.
 * @ingroup app-api
 * */

struct AlloyApp
{
	/** Contains the callbacks for Alloy functions. */
	struct AlloyAppContainer container;
};

/** Initializes the app structure with default values.
 * @param app The app structure to initialize.
 * @ingroup app-api
 * */

void alloy_app_init(struct AlloyApp *app);

/** Set the application host implementation.
 * This will effect how the app will open files,
 * allocate memory, and interface to the operating
 * system in general.
 * @ingroup app-api
 * */

void alloy_app_set_host(struct AlloyApp *app,
                        struct AlloyAppHost *app_host);

/** The entry point of an application.
 * The app container structure has all the function
 * callbacks and data used to implement the standard library.
 * The return value indicates whether the application failed
 * or exited successfully. A return value of zero indicates
 * that the app exited successfully, non-zero means otherwise.
 * @ingroup app-api
 * */

typedef int (*alloy_app_entry)(struct AlloyAppContainer *app_container);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_APP_H */
