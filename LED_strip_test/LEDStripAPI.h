#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef NRF_DRV_SPI_H__
	#include "nrf_drv_spi.h"
#endif

#define LED_SPI_DEFAULT_CONFIG 								 \
{                                                            \
    .sck_pin      = 28,                						 \
    .mosi_pin     = 29,                						 \
    .miso_pin     = NRF_DRV_SPI_PIN_NOT_USED,                \
    .ss_pin       = NRF_DRV_SPI_PIN_NOT_USED,                \
    .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,         \
    .orc          = 0xFF,                                    \
    .frequency    = NRF_DRV_SPI_FREQ_125K,                   \
    .mode         = NRF_DRV_SPI_MODE_0,                      \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,         \
}
#define NUM_LEDS 32

static volatile bool spi_xfer_done;  /* < Flag used to indicate that SPI instance completed the transfer. */
static volatile bool spi_init = false;

static volatile nrf_drv_spi_t* spi_addr;			 /* < master spi instance address, assigned by user */
static const nrf_drv_spi_config_t standard_config = LED_SPI_DEFAULT_CONFIG;
static const uint8_t m_length = NUM_LEDS*3;         /**< Transfer length. */
static uint8_t       m_tx_buf[NUM_LEDS*3];          /**< TX buffer. */
static uint8_t       m_rx_buf[NUM_LEDS*3];    		/**< RX buffer. */

/* rgb_color_t datatype for storing color values*/
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb_color_t;

/* rgb_color_t value used to signify LED turned off */
#define DARK	\
{				\
	.r = 0,		\
	.g = 0,		\
	.b = 0,		\
}

/* BEGIN LINKED LIST HEADER *********************************************************************/
/* NOTE: THE CALLER IS RESPONSIBLE FOR DYNAMIC ALLOCATION!! */

typedef struct node_t {
	rgb_color_t		color;
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

/* END LINKED LIST HEADER ***********************************************************************/

typedef struct led_strip_t {
	uint8_t		id;			// Strip Identifier for eventual MUX Select
	node_t* 	head;		// pointer to linked list representation of LED strip
	uint8_t 	length;		// current length of the linked list

} led_strip_t;

/*

 * Function for initializing SPI interface for specific use with WS2801 LED Strips
 * See LED_SPI_DEFAULT_CONFIG for default configuration of pins and clock frequency

 * @param[in]	p_instance 	Pointer to the driver instance structure.
 */
void led_spi_init(nrf_drv_spi_t const * const p_instance);

/*
 * Function to initiate SPI transfer and display cnages to LED strip

 * @param	strip 	strip to initiate transfer
 */
void show(led_strip_t* strip);


/*
 * Function for setting individual LEDs with a specified color
 * Note: Does not affect other LEDs

 * @param	pos 	position of LED to set
 * @param	color 	rgb_color_t value to set LED to

 */
void setLED(int pos, rgb_color_t color);