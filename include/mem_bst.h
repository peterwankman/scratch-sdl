/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef MEM_BST_H_
#define MEM_BST_H_

#include <stdio.h>

typedef struct mt_data_t {
	size_t n;
	int line;
	char *file;
} mt_data_t;

typedef struct mt_node_t {
	struct mt_node_t *left, *right, *parent;
	void *addr;
	mt_data_t *data;
	int balance;
} mt_node_t;

mt_node_t *mt_lookup_node(void *addr);
mt_data_t *mt_lookup(void *addr);
int mt_ins(void *addr, size_t n, int line, char *file);
int mt_del_node(mt_node_t *node);
void mt_printlist(FILE *fp);

#endif
