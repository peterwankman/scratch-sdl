/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef MEM_H_
#define MEM_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#ifdef LO_DBG_LEAKCHECK
#define malloc(ptr) mem_alloc(ptr, __FILE__, __LINE__)
#define calloc(num, size) mem_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, n) mem_realloc(ptr, n, __FILE__, __LINE__)
#define free mem_free
#elif defined _CRTDBG_MAP_ALLOC
/* Use Visual Studio's internal leak detector if possible. */
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

void *mem_alloc(const size_t n, const char *file, const int line);
void *mem_calloc(const size_t num, const size_t size, const char *file, const int line);
void *mem_realloc(void *ptr, const size_t n, const char *file, const int line);
void mem_free(void *ptr);
size_t get_mem_allocated(void);
void mem_summary(FILE *fp);

#endif