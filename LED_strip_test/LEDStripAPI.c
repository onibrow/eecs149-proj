#include "LEDStripAPI.h"

static int front[NUM_STRIPS];

static void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
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
        // NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
    return;
}

/*
 * Function for initializing SPI interface for specific use with WS2801 LED Strips
 * See LED_SPI_DEFAULT_CONFIG for default configuration of pins and clock frequency

 * @param[in]	p_instance 	Pointer to the driver instance structure.
 */
void led_spi_init(nrf_drv_spi_t const * const p_instance) {
	if (spi_init) {
		printf("ERROR: led spi already initialized1");
		return;
	}
	spi_init = true;
	spi_addr = p_instance;
	APP_ERROR_CHECK(nrf_drv_spi_init(p_instance, &standard_config, spi_event_handler, NULL));
	return;
}

/*
 * Function that takes in an uninitialized led_strip instance and initializes it
 */
void led_strips_init() {
	for (int8_t i = 0; i < NUM_STRIPS; i++) {
		for (int j = 0; j < NUM_LEDS; j++) {
			strips[i][j] = (rgb_color_t) DARK;
		}
	}
	for (int8_t i = 0; i < NUM_STRIPS; i++) {
		front[i] = NUM_LEDS - 1;
	}

	nrf_gpio_cfg_output(MUX_PIN_A);
	nrf_gpio_cfg_output(MUX_PIN_B);
	return;
}

/*
 * Function that takes in a led_strip instance and zeroes it out (frees the linked list)

 * @param 	id 	 	the id of the led_strip instance to free (0 indexed)
 */
void clear_led_strip(int8_t id) {
	for (int i = 0; i < NUM_LEDS; i++) {
		strips[id][i] = (rgb_color_t) DARK;
	}
	front[id] = NUM_LEDS - 1;
	return;
}

/*
 * Function that adds a new light to the beginning of the passed led strip

 * @param 	strip 	address of strip to add next light to
 * @param 	color 	color on next light to add
 */
void push_next_light(int8_t id, rgb_color_t color) {
	strips[id][front[id]] = color;
	if (--(front[id]) < 0) {
		front[id] = NUM_LEDS - 1;
	} 
	return;
}

/*
 * Function to initiate SPI transfer and display cnages to LED strip

 * @param	id 		 	strip to initiate transfer
 */
void show(int8_t id) {
	spi_xfer_done = false;
	nrf_gpio_pin_write(MUX_PIN_A, (id % 2));
	nrf_gpio_pin_write(MUX_PIN_B, ((id >> 1) % 2));

	int i = front[id];
	int count = 0;
	while (count < NUM_LEDS) {
		if (++i >= NUM_LEDS) {
			i = 0;
		}
		m_tx_buf[count*3] = 	strips[id][i].r;
		m_tx_buf[count*3 + 1] = strips[id][i].g;
		m_tx_buf[count*3 + 2] = strips[id][i].b;
		count++;
	}

	APP_ERROR_CHECK(nrf_drv_spi_transfer(spi_addr, m_tx_buf, m_length, m_rx_buf, m_length));

	while (!spi_xfer_done) {
    	__WFE();
    }
    return;
}


/*
 * Function for setting individual LEDs with a specified color
 * Note: Does not affect other LEDs

 * @param	pos 	position of LED to set
 * @param	color 	rgb_color_t value to set LED to

 */
void setLED(int8_t id, int pos, rgb_color_t color) {
	if (pos > NUM_LEDS || pos < 0) {
		printf("ERROR: index out of range!\n");
		return;
	}
	strips[id][pos] = color;
}
