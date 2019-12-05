#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#ifndef NRF_DRV_SPI_H__
	#include "nrf_drv_spi.h"
#endif

#ifndef APP_ERROR_H__
	#include "app_error.h"
#endif

#ifndef NRF_GPIO_H__
	#include "nrf_gpio.h"
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
#define NUM_LEDS 15 		// CHANGE THIS FOR CORRECT NUMBER OF LEDS

#define NUM_STRIPS 3	// CHANGE THIS FOR CORRECT NUMBER OF STRIPS

#define MUX_PIN_A 4 	// CHANGE THIS FOR CORRECT MUX PIN A
#define MUX_PIN_B 3 	// CHANGE THIS FOR CORRECT MUX PIN B

static volatile bool spi_xfer_done;  /* < Flag used to indicate that SPI instance completed the transfer. */
static volatile bool spi_init = false;

static volatile nrf_drv_spi_t* spi_addr;			 /* < master spi instance address, assigned by user */
static const nrf_drv_spi_config_t standard_config = LED_SPI_DEFAULT_CONFIG;
static const uint8_t m_length = NUM_LEDS*3;         /**< Transfer length. */
static volatile uint8_t       m_tx_buf[NUM_LEDS*3];          /**< TX buffer. */
static uint8_t       m_rx_buf[NUM_LEDS*3];    		/**< RX buffer. */

/* rgb_color_t datatype for storing color values */
/* NOTE: colors need to be declared in reverse order (i.e. b, g, r) */
typedef struct rgb_color_t {
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

static rgb_color_t strips[NUM_STRIPS][NUM_LEDS];

/*

 * Function for initializing SPI interface for specific use with WS2801 LED Strips
 * See LED_SPI_DEFAULT_CONFIG for default configuration of pins and clock frequency

 * @param[in]	p_instance 	Pointer to the driver instance structure.
 */
void led_spi_init(nrf_drv_spi_t const * const p_instance);

/*
 * Function that zeros out the LED strips and initializes the MUX
 */
void led_strips_init();

/*
 * Function that takes in a led_strip instance and zeroes it out (frees the linked list)

 * @param 	id 	 	the id of the led_strip instance to free (0 indexed)
 */
void clear_led_strip(int8_t id);

/*
 * Function that adds a new light to the beginning of the passed led strip

 * @param 	strip 	address of strip to add next light to
 * @param 	color 	color on next light to add
 */
void push_next_light(int8_t id, rgb_color_t color);

/*
 * Function to initiate SPI transfer and display cnages to LED strip
 * Returns true when the oldest LED is on

 * @param	id 		 	strip to initiate transfer
 */
bool show(int8_t id);


/*
 * Function for setting individual LEDs with a specified color
 * Note: Does not affect other LEDs

 * @param 	id 		strip id to set LED in
 * @param	pos 	position of LED to set
 * @param	color 	rgb_color_t value to set LED to
 */
void setLED(int8_t id, int pos, rgb_color_t color);


/*
 * Function that prints the current state of the LED strips
 */
void printState();