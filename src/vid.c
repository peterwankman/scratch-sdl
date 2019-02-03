/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <stdio.h>
#include <SDL.h>

#include "mem.h"

#include "defs.h"
#include "main.h"

sdlinfo_t sdlinfo;

int vid_init(const int width, const int height) {
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
		return RET_ERR;
	}
	
	if((sdlinfo.window = SDL_CreateWindow(PROJ_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN)) == NULL) {
		fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
		return RET_ERR;
	}

	if((sdlinfo.renderer = SDL_CreateRenderer(sdlinfo.window, -1, 0)) == NULL) {
		fprintf(stderr, "SDL_CreateRenderer() failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(sdlinfo.window);
		return RET_ERR;
	}

	sdlinfo.width = width;
	sdlinfo.height = height;
	sdlinfo.cursor_x = 0;
	sdlinfo.cursor_y = 0;

	return RET_OK;
}

void vid_quit(void) {
	SDL_DestroyRenderer(sdlinfo.renderer);
	SDL_DestroyWindow(sdlinfo.window);
}