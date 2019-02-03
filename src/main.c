/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifdef _WIN32
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "mem.h"

#include "defs.h"
#include "input.h"
#include "main.h"
#include "vid.h"

extern sdlinfo_t sdlinfo;

static void render(void) {
	SDL_RenderClear(sdlinfo.renderer);
	SDL_RenderPresent(sdlinfo.renderer);
}

int main(int argc, char **argv) {
	time_t start;

	start = time(NULL);

	if(vid_init(WINDOW_WIDTH, WINDOW_HEIGHT) == RET_ERR)
		return EXIT_FAILURE;

	while(time(NULL) - start < 10) {
		get_input();
		dispatch_input();
		render();
	}

	vid_quit();
	
	return EXIT_SUCCESS;
}

int _stdcall WinMain(struct HINSTANCE__ *hInstance, struct HINSTANCE__ *hPrevInstance, char *lpszCmdLine, int nCmdShow) {
#ifdef _WIN32
	HANDLE handle_out, handle_in;
	int hCrt;
	FILE *hf_out, *hf_in;
	char *title;

	/* Spawn console window */
	AllocConsole();

	if((title = malloc(strlen(PROJ_NAME) + strlen(" -- Console") + 1)) != NULL) {
		sprintf(title, "%s -- Console", PROJ_NAME);
		SetConsoleTitleA(title);
		free(title);
	}

	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
	
	handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
#endif

	/* Windows stuff done. Back to a sane environment. */
	return main(__argc, __argv);
}