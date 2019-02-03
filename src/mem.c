/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (_DEBUG) && defined (_CRTDBG_MAP_ALLOC)
#include <crtdbg.h>
#endif

#include "defs.h"
#include "mem_bst.h"

#define CANARY_SIZE	8

static uint8_t mem_canary[CANARY_SIZE] = {
	0x8c, 0xd3, 0x70, 0x29, 0x21, 0xa3, 0x66, 0x78
};

static size_t mem_allocated = 0;
static size_t n_allocs = 0;
static size_t n_frees = 0;

static char *filefrompath(const char *path) {
	size_t idx;

	idx = strlen(path);

	while(idx && path[idx - 1] != '\\' && path[idx - 1] != '/')
		idx--;

	return (char*)path + idx;
}

void mem_free(void *ptr) {
	size_t n;
	mt_node_t *node;
	mt_data_t *data;

	if((node = mt_lookup_node(ptr)) != NULL) {
		data = node->data;
		n = data->n;
		if(memcmp((uint8_t*)node->addr + node->data->n, mem_canary, CANARY_SIZE))
			fprintf(stderr, "WARNING! Buffer Overflow detected! (%s:%d)\n", data->file, data->line);

		mt_del_node(node);
		free(ptr);
		mem_allocated -= n;
		n_frees++;
	}
}

void *mem_alloc(const size_t n, const char *file, const int line) {
	void *new;

	if(n == 0) {
		fprintf(stderr, "Warning: Tried to allocate 0 bytes\n");
		fprintf(stderr, " in file %s, line %d.\n", file, line);
	}

	if((new = malloc(n + CANARY_SIZE)) == NULL)
		return NULL;

	memcpy((uint8_t*)new + n, mem_canary, CANARY_SIZE);

	if(mt_ins(new, n, line, filefrompath(file)) == RET_OK)
		mem_allocated += n;

	n_allocs++;
	return new;
}

void *mem_calloc(const size_t num, const size_t size, const char *file, const int line) {
	void *new;

	if((new = mem_alloc(num * size, file, line)) == NULL)
		return NULL;

	memset(new, 0, num * size);
	return new;
}

void *mem_realloc(void *ptr, const size_t n, const char *file, const int line) {
	void *new = mem_alloc(n, file, line);
	mt_data_t *entry;

	if(ptr)
		entry = mt_lookup(ptr);
	else
		return new;
	
	if(n == 0) {
		mem_free(ptr);
		return NULL;
	}
		
	if(!entry) {
		fprintf(stderr, "Warning: Pointer %p not found in memlist.\n", ptr);		
		mem_free(ptr);
		return NULL;
	}

	if(new) {
		memcpy(new, ptr, entry->n > n ? n : entry->n);
		mem_free(ptr);
		return new;
	}

	mem_free(ptr);
	mem_free(new);
	return NULL;
}

size_t get_mem_allocated(void) { return mem_allocated; }

void mem_summary(FILE *fp) {
#ifdef _DEBUG
	printf("%lu allocs, %lu frees. %lu bytes allocated.\n", n_allocs, n_frees, mem_allocated);
	if(mem_allocated > 0)
		mt_printlist(fp);
#ifdef _CRTDBG_MAP_ALLOC
	_CrtDumpMemoryLeaks();
#endif
#endif
}