#include "LEDStripAPI.h"

void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context);

/**
 * @brief SPI user event handler.
 * @param event
 */
static void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}

/* BEGIN LINKED LIST FUNCTIONS *****************************************************************************************/

/*
 * Inserts a given node at the beginning of the Linked List. 
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

/* 
 * Given a node, frees that node and all subsequent nodes. Does not clear data

 * @param	head 	starting node to begin freeing from
 */
void free_list(node_t* head) {
	if (head != NULL) {
		node_t* ptr = head;
		node_t* next = ptr->next;

		while (next != NULL) {
			free(ptr);
			ptr = next;
			next = ptr.next;
		}

		free(ptr);
	}
	return;
}
/* END LINKED LIST FUNCTIONS *****************************************************************************************/

/*
 * 
  */
void led_spi_init(nrf_drv_spi_t const * const p_instance) {
	if (spi_init) {
		printf("ERROR: led spi already initialized1");
		return
	}
	spi_init = true;
	spi_addr = p_instance;
	APP_ERROR_CHECK(nrf_drv_spi_init(p_instance, &standard_config, spi_event_handler, NULL));
	return;
}

/*
 * Function that takes in an uninitialized led_strip instance and initializes it

 * @param 	strip 	the address of an uninitialized led_strip instance
 * @param 	id 		unique identifier of the led strip (best used as mux index)
 */
void led_strip_init(led_strip_t* strip, uint8_t id) {
	strip->id = id;
	strip->head = NULL;
	strip->length = 0;
	return;
}

/*
 * Function that takes in a led_strip instance and zeroes it out (frees the linked list)

 * @param 	strip 	the address of the led_strip instance to free
 */
void clear_led_strip(led_strip_t* strip) {
	free_list(strip->head);
	strip->length = 0;
	strip->head = NULL;
	return;
}

		// DECIDED NOT TO USE DYNAMIC ALLOCATION FOR led_strip_t DATA STRUCT

		// led_strip_t* new_led_strip_init(uint8_t id) {
		// 	led_strip_t* strip = (led_strip_t*) malloc(sizeof(led_strip_t));
		// 	strip->id = id;
		// 	strip->head = NULL;
		// 	strip->length = 0;

		// 	return strip;
		// }

		// /*
		//  * Deinitializes the given led strip instance altogether

		//  * @param	strip 	the led_strip_t instance to deinitialize
		//  */
		// void led_strip_deinit(led_strip_t* strip) {
		// 	free_list(strip->head);
		// 	strip->length = 0;
		// 	strip->head = NULL;
		// 	free(strip);
		// }

void push_next_light(led_strip_t* strip, rgb_color_t color) {
	return;
}

void show(led_strip_t* strip) {
	// nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length)
	return 0;
}