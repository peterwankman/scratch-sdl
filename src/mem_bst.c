/*
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <stdlib.h>

#include "defs.h"
#include "mem_bst.h"

mt_node_t *mt_root = NULL;

/* MT_LOOKUP */

static mt_node_t *lookup_rec(mt_node_t *root, void *addr) {
	if(root == NULL) return NULL;
	if(root->addr == addr) return root;

	if(addr < root->addr)
		return lookup_rec(root->left, addr);
	if(addr > root->addr)
		return lookup_rec(root->right, addr);

	return NULL;
}

mt_node_t *mt_lookup_node(void *addr) {
//	mt_node_t *node;

//	if((node = lookup_rec(mt_root, addr)) == NULL)
//		return NULL;
	return lookup_rec(mt_root, addr);
}

mt_data_t *mt_lookup(void *addr) {
	mt_node_t *node;

	if((node = lookup_rec(mt_root, addr)) == NULL)
		return NULL;

	return node->data;
}

/* MT_REBALANCE */

static void rot_l(mt_node_t *node) {
	mt_node_t *oldparent = node->parent;
	mt_node_t *oldleft = node->left;
	mt_node_t *grandparent = oldparent->parent;
	int balance = node->balance;

	if(mt_root == oldparent)
		mt_root = node;

	if(oldleft)
		oldleft->parent = oldparent;

	node->parent = grandparent;
	oldparent->right = oldleft;
	oldparent->parent = node;
	node->left = oldparent;

	if(balance == 0) {
		oldparent->balance = 1;
		node->balance = -1;
	} else {
		oldparent->balance = 0;
		node->balance = 0;
	}

	if(grandparent) {
		if(grandparent->left == oldparent)
			grandparent->left = node;
		else
			grandparent->right = node;
	}
}

static void rot_r(mt_node_t *node) {
	mt_node_t *oldparent = node->parent;
	mt_node_t *oldright = node->right;
	mt_node_t *grandparent = oldparent->parent;
	int balance = node->balance;

	if(mt_root == oldparent)
		mt_root = node;

	if(oldright)
		oldright->parent = oldparent;

	node->parent = grandparent;
	oldparent->left = oldright;
	oldparent->parent = node;
	node->right = oldparent;

	if(balance == 0) {
		oldparent->balance = -1;
		node->balance = 1;
	} else {
		oldparent->balance = 0;
		node->balance = 0;
	}

	if(grandparent) {
		if(grandparent->left == oldparent)
			grandparent->left = node;
		else
			grandparent->right = node;
	}
}

static void rot_lr(mt_node_t *node) {
	mt_node_t *oldleft = node->left;
	mt_node_t *innerchild = oldleft->right;
	int innerbalance = innerchild->balance;

	rot_l(innerchild);
	rot_r(innerchild);
	
	if(innerbalance < 0) {
		node->balance = 1;
		oldleft->balance = 0;
	} else if(innerbalance == 0) {
		node->balance = 0;
		oldleft->balance = 0;
	} else {
		node->balance = 0;
		oldleft->balance = -1;
	}
	innerchild->balance = 0;
}

static void rot_rl(mt_node_t *node) {	
	mt_node_t *oldright = node->right;
	mt_node_t *innerchild = oldright->left;
	int innerbalance = innerchild->balance;

	rot_r(innerchild);
	rot_l(innerchild);

	if(innerbalance > 0) {
		node->balance = -1;
		oldright->balance = 0;
	} else if(innerbalance == 0) {
		node->balance = 0;
		oldright->balance = 0;
	} else {
		node->balance = 0;
		oldright->balance = 1;
	}
	innerchild->balance = 0;
}

static mt_node_t *rebal(mt_node_t *unbal, int *dbl) {
	mt_node_t *out;

	if(unbal->balance == -2) {
		if(unbal->left->balance < 1) {
			rot_r(unbal->left);
			*dbl = 0;
			out = unbal;
		} else if(unbal->left->balance == 1) {
			rot_lr(unbal);
			*dbl = 1;
			out = unbal->parent;
		} 
	} else {
		if(unbal->right->balance > -1) {
			rot_l(unbal->right);
			*dbl = 0;
			out = unbal;
		} else if(unbal->right->balance == -1) {
			rot_rl(unbal);
			*dbl = 1;
			out = unbal->parent;
		} 
	}

	return out;
}

static void rebal_insert(mt_node_t *node, mt_node_t *child) {
	int dbl;

	if(child == NULL)
		node->balance = 0;
	else {
		if(child == node->left)
			node->balance--;
		else
			node->balance++;
	}

	if(node->balance == -2 || node->balance == 2)
		node = rebal(node, &dbl);
	
	if(node->balance == 0) return;

	if(node->parent)
		rebal_insert(node->parent, node);
}

/* MT_INSERT */

static int bst_ins(mt_node_t *root, mt_node_t *newnode) {
	if(mt_root == NULL) {
		mt_root = newnode;
		return RET_OK;
	}

	if(newnode->addr > root->addr) {
		if(root->right == NULL) {
			root->right = newnode;
			newnode->parent = root;
			return RET_OK;
		}	
		return bst_ins(root->right, newnode);
	} else if(newnode->addr < root->addr) {
		if(root->left == NULL) {
			root->left = newnode;
			newnode->parent = root;
			return RET_OK;
		}
		return bst_ins(root->left, newnode);
	}

	/* DUPE! */
	return RET_ERR;
}

/* GENERIC */

static mt_data_t *data_new(size_t n, int line, char *file) {
	mt_data_t *out;
	
	if((out = malloc(sizeof(mt_data_t))) == NULL)
		return NULL;

	out->n = n;
	out->line = line;
	out->file = file;

	return out;
}

int mt_ins(void *addr, size_t n, int line, char *file) {
	mt_node_t *newnode;
	mt_data_t *data;
	int ret;
	
	if(addr == NULL) return RET_ERR;
	if((data = data_new(n, line, file)) == NULL) return RET_ERR;

	if((newnode = malloc(sizeof(mt_node_t))) == NULL) {
		free(data);
		return RET_ERR;
	}

	newnode->addr = addr;
	newnode->balance = 0;
	newnode->data = data;
	newnode->left = newnode->parent = newnode->right = NULL;

	if((ret = bst_ins(mt_root, newnode)) == RET_OK) {
		if(newnode->parent) {
			rebal_insert(newnode->parent, newnode);
		}
	} else {
		free(newnode);
		free(data);
	}

	return ret;
}

/* MT_DELETE */

static mt_node_t *fixparent(mt_node_t *node, mt_node_t *parent, mt_node_t *child_left, mt_node_t *child_right) {
	mt_node_t *child, *ret;

	child = (child_left == NULL) ? child_right : child_left;

	if(child == NULL) {
		ret = parent;
	} else {
		ret = child;
		child->parent = parent;
	}

	if(parent == NULL) {
		mt_root = child;
		return child;
	}

	if(node == parent->left) {
		parent->left = child;
	} else {
		parent->right = child;
	}

	return ret;
}

static mt_node_t *pred(mt_node_t *node) {
	node = node->left;

	while(node->right)
		node = node->right;

	return node;
}

static void rebal_del(mt_node_t *node, mt_node_t *child) {
	mt_node_t *parent;
	int dbl;

	if(node == NULL)
		return;

	if((node->left == NULL) && (node->right == NULL)) {
		node->balance = 0;
		rebal_del(node->parent, node);
		return;
	} 

	if(child == node->left) {
		node->balance += 1;
	} else if(child == node->right) {
		node->balance -= 1;
	}

	if((node->balance < -1) || (node->balance > 1)) {
		parent = rebal(node, &dbl);

		if((parent->balance == -1) || (parent->balance == 1))
			return;

		if(dbl == 0)
			rebal_del(parent->parent->parent, parent->parent);
		else
			rebal_del(parent->parent, parent);

	} else if(node->balance == 0) {
		rebal_del(node->parent, node);
	}
}

static mt_node_t *bst_del(mt_node_t *node) {
	mt_node_t *parent, *predecessor;
	mt_node_t *ret;
	void *tempaddr;
	mt_data_t *tempdata;

	parent = node->parent;

	if((node->left == NULL) || (node->right == NULL)) {
		ret = fixparent(node, parent, node->left, node->right);
		free(node->data);
		free(node);
	} else {
		predecessor = pred(node);
		tempaddr = node->addr;
		tempdata = node->data;

		node->addr = predecessor->addr;
		node->data = predecessor->data;

		predecessor->addr = tempaddr;
		predecessor->data = tempdata;

		return bst_del(predecessor);
	}

	return ret;
}

int mt_del_node(mt_node_t *node) {
	if(mt_root == NULL) return RET_ERR;
	if(node == NULL) return RET_ERR;

	if((node = bst_del(node)) == NULL) {
		if(mt_root == NULL)
			return RET_OK;
		return RET_ERR;
	}

	rebal_del(node, NULL);
	return RET_OK;
}

/* MT_ETC */
static void printlist_rec(FILE *fp, mt_node_t *node) {
	mt_data_t *data;

	if(node == NULL) return;

	if((data = node->data) == NULL)
		fprintf(fp, "ERROR: Empty memtree node!\n");
	else
		fprintf(fp, "%s:%d %lu bytes at %p.\n", data->file, data->line, data->n, node->addr);

	printlist_rec(fp, node->left);
	printlist_rec(fp, node->right);
}

void mt_printlist(FILE *fp) {
	fprintf(fp, "Still allocated:\n");
	printlist_rec(fp, mt_root);
}
