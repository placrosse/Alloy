#include <alloy/sdl-term.h>

#include <alloy/types.h>

const struct AlloyTerm alloy_sdl_term = {
	ALLOY_NULL /* init */,
	ALLOY_NULL /* done */,
	ALLOY_NULL /* clear */,
	ALLOY_NULL /* get_cursor_pos */,
	ALLOY_NULL /* get_screen_size */,
	ALLOY_NULL /* write */,
	ALLOY_NULL /* set_background */,
	ALLOY_NULL /* set_cursor_pos */,
	ALLOY_NULL /* set_foreground */
};
