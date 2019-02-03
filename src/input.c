/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <crtdbg.h>
#endif

#include <SDL.h>

#include "mem.h"

#include "defs.h"
#include "input.h"

extern sdlinfo_t sdlinfo;

static input_queue_t *queue_start = NULL;
static input_queue_t *queue_end = NULL;

static key_input_t *get_keys(SDL_KeyboardEvent ev, Uint32 type) {
	key_input_t *out;

	if((out = malloc(sizeof(key_input_t))) == NULL)
		return NULL;
	
	switch(type) {
		case SDL_KEYDOWN:	out->type = DOWN;	break;
		case SDL_KEYUP:		out->type = UP;		break;
		default:			out->type = NONE;
	}

	out->Keysym = ev.keysym;

	return out;
}

static mouse_input_t *get_mouse_button(SDL_MouseButtonEvent ev, Uint32 type) {
	mouse_input_t *out;

	if((out = malloc(sizeof(mouse_input_t))) == NULL)
		return NULL;

	switch(type) {
		case SDL_MOUSEBUTTONUP:		out->dir = UP;		break;
		case SDL_MOUSEBUTTONDOWN:	out->dir = DOWN;	break;
		default:					out->dir = NONE;
	}

	out->type = BUTTON;
	out->Button = ev;

	return out;
}

static mouse_input_t *get_mouse_motion(SDL_MouseMotionEvent ev) {
	mouse_input_t *out;

	if((out = malloc(sizeof(mouse_input_t))) == NULL)
		return NULL;
	
	out->dir = NONE;
	out->type = MOTION;
	out->Motion = ev;
	return out;
}

static mouse_input_t *get_mouse_wheel(SDL_MouseWheelEvent ev) {
	mouse_input_t *out;

	if((out = malloc(sizeof(mouse_input_t))) == NULL)
		return NULL;

	if(ev.y > 0)
		out->dir = UP;
	else if(ev.y < 0)
		out->dir = DOWN;
	else
		out->dir = NONE;

	out->type = WHEEL;
	out->Wheel = ev;
	return out;
}

static int add_to_queue(input_t *input) {
	input_queue_t *newent;

	if((newent = malloc(sizeof(input_queue_t))) == NULL)
		return 0;

	newent->event = input;
	newent->next = NULL;

	if(queue_end == NULL) {
		queue_start = newent;
	} else {
		queue_end->next = newent;
	}

	queue_end = newent;
	return 1;
}

void get_input(void) {
	SDL_Event ev;
	input_t *input;
	key_input_t *key_input;
	mouse_input_t *mouse_input;

	while(SDL_PollEvent(&ev)) {
		input = NULL;
		key_input = NULL;
		mouse_input = NULL;

		switch(ev.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				key_input = get_keys(ev.key, ev.type);
				break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				mouse_input = get_mouse_button(ev.button, ev.type);
				break;
			case SDL_MOUSEMOTION:
				mouse_input = get_mouse_motion(ev.motion);
				break;
			case SDL_MOUSEWHEEL:
				mouse_input = get_mouse_wheel(ev.wheel);
				break;
		}

		if(key_input) {
			if((input = malloc(sizeof(input_t))) == NULL) {
				free(key_input);
				continue;
			}
			input->type = KEYBOARD;
			input->key = key_input;
		} else if(mouse_input) {
			if((input = malloc(sizeof(input_t))) == NULL) {
				free(mouse_input);
				continue;
			}
			input->type = MOUSE;
			input->mouse = mouse_input;
		}

		if(input) {
			add_to_queue(input);
		}
	}
}

static int default_consume_mouse(mouse_input_t *input) {
	printf("Unconsumed mouse input. ");
	switch(input->type) {
		case BUTTON:
			printf("TYPE: Button ");
			break;
		case WHEEL:
			printf("TYPE: Wheel ");
			break;
		default:
			printf("TYPE: Unknown ");
	}

	switch(input->dir) {
		case UP:
			printf("up\n");
			break;
		case DOWN:
			printf("down\n");
			break;
		case NONE:
			printf("\n");
			break;
	}

	return INPUT_CONSUMED;
}

static int default_consume_keyboard(key_input_t *input) {
	printf("Unconsumed keyboard input. ");
	switch(input->type) {
		case UP:
			printf("TYPE: Up\n");
			break;
		case DOWN:
			printf("TYPE: Down\n");
			break;
		default:
			printf("TYPE: Unknown\n");
	}

	return INPUT_CONSUMED;
}

static int dispatch_keyboard(key_input_t *key_input) {
	int ret = INPUT_IGNORED;

	if(key_input == NULL)
		return INPUT_IGNORED;

	/*
		if((ret = target1_consume_keyboard(key_input)) == INPUT_CONSUMED) goto done;
		...
	*/
	if((ret = default_consume_keyboard(key_input)) == INPUT_CONSUMED) goto done;

done:
	free(key_input);
	return ret;
}

static int dispatch_mouse(mouse_input_t *mouse_input) {
	int ret = INPUT_IGNORED;

	if(mouse_input == NULL)
		return ret;

	if(mouse_input->type != MOTION) {
		/*
		if((ret = target1_consume_mouse(mouse_input)) == INPUT_CONSUMED) goto done;
		...
		*/
		if((ret = default_consume_mouse(mouse_input)) == INPUT_CONSUMED) goto done;
	} else {
		sdlinfo.cursor_x = mouse_input->Motion.x;
		sdlinfo.cursor_y = mouse_input->Motion.y;
		
		/*
		target1_notify_movement();
		...
		*/
	}

done:
	free(mouse_input);
	return ret;
}

void dispatch_input(void) {
	input_queue_t *current;
	input_t *input;

	while(queue_start != NULL) {
		current = queue_start;
		input = current->event;
		
		if(input) {
			if(input->type == KEYBOARD) {
				dispatch_keyboard(input->key);
			} else if(input->type == MOUSE) {
				dispatch_mouse(input->mouse);
			} free(input);
		}

		if(current == queue_end)
			queue_end = NULL;

		queue_start = queue_start->next;
		free(current);
	}
}
