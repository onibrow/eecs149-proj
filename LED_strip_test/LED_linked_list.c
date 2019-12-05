#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "LED_linked_list.h"

/*** NOTICE: THIS CLASS DOES NOT IMPLEMENT ANY DYNAMIC ALLOCATION. MALLOCS AND FREES MUST BE HANDLED MANUALLY ***/


/*
 * Inserts a node at the beginning of the Linked List. 
 * If list is NULL, a new node is created and returned.

 * @param	list 	The linked list to add to. If NULL, a new list is created and returned
 * @param	node 	Node to add to the given linked list
 */
node_t* insert_first(node_t* list, node_t* node) {
	// if node is empty, fault
	if (node == NULL) {
        printf("\n***\nERROR: node passed into `insert_first` was NULL!!\n***\n");
        nrf_delay_ms(100);
        APP_ERROR_CHECK(NRF_ERROR_NULL);
	}

	// node becomes the first element in the list and is returned
	node->next = list;
	return node;
}

/*
 * Pops the last value in the Linked List
 */
node_t* remove_last(node_t* list) {
	// if no list given, return NULL
	if (list == NULL) {
		return NULL;
	}

	node_t* ptr = list;
	node_t* prev = list;

	// loop to get to last element in the linked list
	while (ptr->next != NULL) {
		prev = ptr;
		ptr = ptr->next;
	}

	// make the node before the last into the new ending node of the linked list
	// if the list was of size one, there is no issue, it is simply returned
	prev->next = NULL;
	return ptr;
}

/* 
 * Gets the node at the indicated position
 * NOTE: If the indicated position is longer than the length of the linked list,
 * 			the last value in the linked list is returned.
 */
node_t* get_node_at(node_t* list, int pos) {
	// If list is empty or pos negative, return NULL
	if (list == NULL || pos < 0) {
		return NULL;
	}

	int i = 0;
	node_t* ptr = list;
	while(i < pos && ptr->next != NULL) {
		ptr = ptr->next;
		i++;
	}
	return ptr;
}

