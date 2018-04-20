
/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/app.h>

#include <alloy/string.h>

int alloy_app_load_bin(struct AlloyApp *app,
                       const void *app_data_ptr,
                       alloy_size app_size)
{
	void *dst = (void *) 0xffff800000000000;

	alloy_memcpy(dst, app_data_ptr, app_size);

	app->entry = (alloy_app_entry) dst;

	return 0;
}
