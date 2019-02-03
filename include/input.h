/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <SDL.h>

#define INPUT_CONSUMED	1
#define INPUT_SHARED	2
#define INPUT_IGNORED	3

typedef enum mouse_input_type_t { BUTTON, MOTION, WHEEL } mouse_input_type_t;
typedef enum key_input_type_t { UP, DOWN, NONE } key_input_type_t;
typedef enum input_type_t { KEYBOARD, MOUSE } input_type_t;

typedef struct mouse_input_t {
	mouse_input_type_t type;
	key_input_type_t dir;
	union {
		SDL_MouseButtonEvent Button;
		SDL_MouseMotionEvent Motion;
		SDL_MouseWheelEvent Wheel;
	};
} mouse_input_t;

typedef struct key_input_t {
	key_input_type_t type;
	SDL_Keysym Keysym;
} key_input_t;

typedef struct input_t {
	input_type_t type;
	union {
		mouse_input_t *mouse;
		key_input_t *key;
	};
} input_t;

typedef struct input_queue_t {
	input_t *event;
	struct input_queue_t *next;
} input_queue_t;

void get_input(void);
void dispatch_input(void);

#endif