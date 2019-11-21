#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "LEDStripAPI.h"

typedef struct node_t {
	rgb_color		color;
	struct node_t* 	next;
} node_t;

/*
 * Inserts a node at the beginning of the Linked List. 
 * If list is NULL, a new node is created and returned.

 * @param	list 	The linked list to add to. If NULL, a new list is created and returned
 * @param	node 	Node to add to the given linked list
 */
node_t* insert_first(node_t* list, node_t* node);

// Pops the last value in the Linked List and returns it
node_t* remove_last(node_t* list);

// Gets the node at the indicated position and returns it (0 indexed)
node_t* get_node_at(node_t* list, int pos);

