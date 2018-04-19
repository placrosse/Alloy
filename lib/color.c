/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/color.h>

#include <alloy/errno.h>
#include <alloy/string.h>

static int parse_channel(alloy_uint8 *channel,
                         const char *str)
{
	*channel = 0;

	if ((str[0] >= '0') && (str[0] <= '9'))
		*channel += (str[0] - '0');
	else if ((str[0] >= 'a') && (str[0] <= 'f'))
		*channel += 10 + (str[0] - 'a');
	else if ((str[0] >= 'A') && (str[0] <= 'F'))
		*channel += 10 + (str[0] - 'A');
	else
		return ALLOY_EINVAL;

	*channel <<= 4;

	if ((str[0] >= '0') && (str[0] <= '9'))
		*channel += (str[0] - '0');
	else if ((str[0] >= 'a') && (str[0] <= 'f'))
		*channel += 10 + (str[0] - 'a');
	else if ((str[0] >= 'A') && (str[0] <= 'F'))
		*channel += 10 + (str[0] - 'A');
	else
		return ALLOY_EINVAL;

	return 0;
}

static int parse_name(struct AlloyColor *color,
                      const char *str)
{
	if (alloy_strcmp(str, "red") == 0)
		*color = alloy_red;
	else if (alloy_strcmp(str, "green") == 0)
		*color = alloy_green;
	else if (alloy_strcmp(str, "blue") == 0)
		*color = alloy_blue;
	else if (alloy_strcmp(str, "white") == 0)
		*color = alloy_white;
	else if (alloy_strcmp(str, "black") == 0)
		*color = alloy_black;

	return 0;
}

int alloy_color_parse(struct AlloyColor *color,
                      const char *str)
{
	int err = parse_name(color, str);
	if (err == 0)
		return 0;

	if (str[0] == '#')
		str = &str[1];

	err = parse_channel(&color->red, &str[0]);
	if (err != 0)
		return err;

	err = parse_channel(&color->green, &str[2]);
	if (err != 0)
		return err;

	err = parse_channel(&color->blue, &str[4]);
	if (err != 0)
		return err;

	return 0;
}

const struct AlloyColor alloy_red = {
	0xff,
	0x00,
	0x00
};

const struct AlloyColor alloy_green = {
	0x00,
	0xff,
	0x00
};

const struct AlloyColor alloy_blue = {
	0x00,
	0x00,
	0xff
};

const struct AlloyColor alloy_white = {
	0xff,
	0xff,
	0xff
};

const struct AlloyColor alloy_black = {
	0x00,
	0x00,
	0x00
};
