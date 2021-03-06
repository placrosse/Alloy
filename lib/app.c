/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/app.h>

#include <alloy/errno.h>
#include <alloy/host.h>
#include <alloy/term.h>
#include <alloy/types.h>

#define ALLOY_STDIN 0
#define ALLOY_STDOUT 1
#define ALLOY_STDERR 2

static int app_read_stdin(void *buf,
                          unsigned int len,
                          struct AlloyAppHost *app_host)
{
	alloy_utf8 *buf8 = (alloy_utf8 *) buf;

	for (unsigned int i = 0; i < len; i++)
	{
		alloy_utf8 c = 0;

		int err = alloy_term_get_char(app_host->term,
		                              app_host->term_data,
		                              &c);
		if (err != 0)
			return err;

		buf8[i] = c;
	}

	return len;
}

static int app_read(int fd,
                    void *buf,
                    unsigned int len,
                    struct AlloyAppHost *app_host)
{
	if (fd == ALLOY_STDIN)
	{
		if (app_host->term == ALLOY_NULL)
			return -1;

		return app_read_stdin(buf, len, app_host);
	}
	else
	{
		/* Not implemented yet. */
		return -1;
	}

	return (int) len;
}

static int app_write(int fd,
                     const void *buf,
                     unsigned int len,
                     struct AlloyAppHost *app_host)
{
	if (app_host == ALLOY_NULL)
		return -1;

	if ((fd == ALLOY_STDOUT)
	 || (fd == ALLOY_STDERR))
	{
		if (app_host->term == ALLOY_NULL)
			return -1;

		int err = alloy_term_write(app_host->term,
		                           app_host->term_data,
		                           buf, len);
		if (err != 0)
			return -1;
	}
	else
	{
		/* Not implemented yet. */
		return -1;
	}

	return (int) len;
}

static int app_open(const char *path,
                    int mode,
                    struct AlloyAppHost *app_host)
{
	/* TODO */
	(void) path;
	(void) mode;
	(void) app_host;
	return -1;
}

static int app_close(int fd, struct AlloyAppHost *app_host)
{
	/* TODO */
	(void) fd;
	(void) app_host;
	return -1;
}

static void *app_malloc(unsigned long int size,
                        struct AlloyAppHost *app_host)
{
	/* TODO */
	(void) size;
	(void) app_host;
	return ALLOY_NULL;
}

static void *app_realloc(void *addr,
                         unsigned long int size,
                         struct AlloyAppHost *app_host)
{
	/* TODO */
	(void) addr;
	(void) size;
	(void) app_host;
	return ALLOY_NULL;
}

static void app_free(void *addr,
                     struct AlloyAppHost *app_host)
{
	/* TODO */
	(void) addr;
	(void) app_host;
}

void alloy_app_init(struct AlloyApp *app)
{
	app->container.host = ALLOY_NULL;
	app->container.argc = 0;
	app->container.argv = ALLOY_NULL;
	app->container.env = ALLOY_NULL;
	app->container.write = app_write;
	app->container.read = app_read;
	app->container.open = app_open;
	app->container.close = app_close;
	app->container.malloc = app_malloc;
	app->container.realloc = app_realloc;
	app->container.free = app_free;
	app->entry = ALLOY_NULL;
	app->exitcode = 0;
}

void alloy_app_set_host(struct AlloyApp *app,
                        struct AlloyAppHost *app_host)
{
	if (app_host != ALLOY_NULL)
		app->container.host = app_host;
}

void alloy_app_set_args(struct AlloyApp *app,
                        int argc,
                        char **argv)
{
	app->container.argc = argc;
	app->container.argv = argv;
}

int alloy_app_load(struct AlloyApp *app,
                   const void *app_data_ptr,
                   alloy_size app_size)
{
	const unsigned char *app_data = (const unsigned char *) app_data_ptr;

	if ((app_size >= 4)
	 && (app_data[0] == 0x7f)
	 && (app_data[1] == 'E')
	 && (app_data[2] == 'L')
	 && (app_data[3] == 'F'))
	{
		return alloy_app_load_elf(app, app_data_ptr, app_size);
	}
	else
	{
		return alloy_app_load_bin(app, app_data_ptr, app_size);
	}
}

int alloy_app_run(struct AlloyApp *app)
{
	if (app->entry == ALLOY_NULL)
		return ALLOY_EFAULT;

	app->exitcode = app->entry(&app->container);

	return 0;
}
