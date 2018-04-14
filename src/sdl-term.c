/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include "term.h"

#include <alloy/keys.h>
#include <alloy/types.h>

#include <SDL.h>

#include <stdio.h>

#ifndef ALLOY_SCREEN_WIDTH
#define ALLOY_SCREEN_WIDTH 640
#endif

#ifndef ALLOY_SCREEN_HEIGHT
#define ALLOY_SCREEN_HEIGHT 480
#endif

struct AlloyTermData
{
	SDL_Window *Window;
	SDL_Surface *Surface;
};

static struct AlloyTermData *sdl_term_init(void)
{
	printf("HERE\n");

	struct AlloyTermData *term_data = malloc(sizeof(*term_data));
	if (term_data == NULL)
		return ALLOY_NULL;

	term_data->Window = ALLOY_NULL;
	term_data->Surface = ALLOY_NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		free(term_data);
		return ALLOY_NULL;
	}

	term_data->Window = SDL_CreateWindow("Alloy",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		ALLOY_SCREEN_WIDTH,
		ALLOY_SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (term_data->Window == NULL)
	{
		free(term_data);
		return ALLOY_NULL;
	}

	return term_data;
}

static void sdl_term_done(struct AlloyTermData *term_data)
{
	if (term_data == ALLOY_NULL)
		return;

	if (term_data->Window != ALLOY_NULL)
		SDL_DestroyWindow(term_data->Window);

	SDL_Quit();

	free(term_data);
}

static alloy_utf8 sdl_term_to_char(SDL_Keycode key)
{
	switch (key)
	{
	case SDLK_RETURN:
		return '\n';
	default:
		return ' ';
	}
}

static int sdl_term_get_char(struct AlloyTermData *term_data,
                             alloy_utf8 *c)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			*c = ALLOY_KEY_QUIT;
			break;
		case SDL_KEYDOWN:
			*c = sdl_term_to_char(event.key.keysym.sym);
			break;
		default:
			break;
		}
	}

	(void) term_data;

	return 0;
}

const struct AlloyTerm alloy_term = {
	sdl_term_init,
	sdl_term_done,
	ALLOY_NULL /* clear */,
	sdl_term_get_char,
	ALLOY_NULL /* get_cursor_pos */,
	ALLOY_NULL /* get_screen_size */,
	ALLOY_NULL /* write */,
	ALLOY_NULL /* set_background */,
	ALLOY_NULL /* set_cursor_pos */,
	ALLOY_NULL /* set_foreground */
};
