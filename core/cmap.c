#include "cmap.h"


struct mynode * my_search(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;

	while (node) {
		struct mynode *data = container_of(node, struct mynode, node);
		int result = key - data->key;
		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int my_insert(struct rb_root *root, struct mynode *data)
{
	struct rb_node **new_node = &(root->rb_node), *parent = NULL;

	while (*new_node) {
		struct mynode *this_node = container_of(*new_node, struct mynode, node);
		int result = data->key - this_node->key;

		parent = *new_node;
		if (result < 0)
			new_node = &((*new_node)->rb_left);
		else if (result > 0)
			new_node = &((*new_node)->rb_right);
		else
			return 0;
	}

	rb_link_node(&data->node, parent, new_node);
	rb_insert_color(&data->node, root);

	return 1;
}

void my_erase(struct rb_root *root, struct mynode *node, free_elem free_func)
{
	rb_erase(&node->node, root);
	if (node != NULL) {
		if (node->elem != NULL) {
			free_func(node->elem);
			node->elem = NULL;
		}
		free(node);
		node = NULL;
	}
}


