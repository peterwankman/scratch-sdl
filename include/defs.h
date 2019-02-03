/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <SDL.h>

#define RET_ERR		0
#define RET_OK		1

typedef struct sdlinfo_t {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int width, height;
	int cursor_x, cursor_y;
} sdlinfo_t;

#endif