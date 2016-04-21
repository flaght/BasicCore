#ifndef LIBRARY_CORE_CMAP_H_
#define LIBRARY_CORE_CMAP_H_

#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct mynode {
	struct rb_node node;
	int key;
	void* elem;
};

typedef void(*free_elem)(void* elem);

extern struct mynode * my_search(struct rb_root *root, int key);

extern int my_insert(struct rb_root *root, struct mynode *data);

extern void my_erase(struct rb_root *root, struct mynode *node, free_elem free_func);

#endif /* LIBRARY_CORE_CMAP_H_ */
