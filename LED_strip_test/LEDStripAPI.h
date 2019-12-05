#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "LED_linked_list.h"

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

static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

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


/*

 * Function for initializing SPI interface for specific use with WS2801 LED Strips
 * See LED_SPI_DEFAULT_CONFIG for default configuration of pins and clock frequency

 * @param[in] p_instance Pointer to the driver instance structure.
 * @param     handler    Event handler provided by the user. If NULL, transfers
 *                       will be performed in blocking mode.
 * @param     p_context  Context passed to event handler.

 * @retval NRF_SUCCESS             If initialization was successful.
 * @retval NRF_ERROR_INVALID_STATE If the driver was already initialized.
 * @retval NRF_ERROR_BUSY          If some other peripheral with the same
 *                                 instance ID is already in use. This is
 *                                 possible only if PERIPHERAL_RESOURCE_SHARING_ENABLED
 *                                 is set to a value other than zero.
 */
ret_code_t led_spi_init(nrf_drv_spi_t const * const p_instance,
						nrf_drv_spi_evt_handler_t    handler,
                        void *                       p_context);

/*
 * Function to initiate SPI transfer and display cnages to LED strip

 * @retval NRF_SUCCESS            If the operation was successful.
 * @retval NRF_ERROR_BUSY         If a previously started transfer has not finished
 *                                yet.
 * @retval NRF_ERROR_INVALID_ADDR If the provided buffers are not placed in the Data
 *                                RAM region.
 */
ret_code_t show();


/*
 * Function for setting individual LEDs with a specified color
 * Note: Does not affect other LEDs

 * @param	pos 	position of LED to set
 * @param	color 	rgb_color_t value to set LED to

 */
void setLED(int pos, rgb_color_t color);