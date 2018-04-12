/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/host.h>

struct AlloyHostData *alloy_host_init(const struct AlloyHost *host)
{
	if (host->init == ALLOY_NULL)
		return ALLOY_NULL;
	else
		return host->init();
}

void alloy_host_done(const struct AlloyHost *host,
                     struct AlloyHostData *host_data)
{
	if (host->done != ALLOY_NULL)
		host->done(host_data);
}

