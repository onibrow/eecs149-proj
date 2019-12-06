#include "LEDStripAPI.h"

static int last[NUM_STRIPS];

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
    // printf("Transfer Done!\n");
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
		last[i] = NUM_LEDS - 1;
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
	last[id] = NUM_LEDS - 1;
	return;
}

/*
 * Function that adds a new light to the beginning of the passed led strip

 * @param 	strip 	address of strip to add next light to
 * @param 	color 	color on next light to add
 */
void push_next_light(int8_t id, rgb_color_t color) {
	strips[id][last[id]] = color;
	if (--(last[id]) < 0) {
		last[id] = NUM_LEDS - 1;
	} 
	return;
}

/*
 * Function to initiate SPI transfer and display cnages to LED strip
 * Returns true when the oldest LED is on

 * @param	id 		 	strip to initiate transfer
 */
void show(int8_t id) {
	spi_xfer_done = false;
	nrf_gpio_pin_write(MUX_PIN_A, (id % 2));
	nrf_gpio_pin_write(MUX_PIN_B, ((id >> 1) % 2));

	int i = last[id];
	int j = 0;
	int count = NUM_LEDS-1;
	while (count >= 0) {
		if (++i >= NUM_LEDS) {
			i = 0;
		}
		m_tx_buf[count*3] = 	strips[id][i].b;
		m_tx_buf[count*3 + 1] = strips[id][i].g;
		m_tx_buf[count*3 + 2] = strips[id][i].r;

		if (count < HZ_SIZE) {
			hit_zone[id][j] = (strips[id][i].b != 0) || (strips[id][i].g != 0) || (strips[id][i].r != 0);
			j++;
		}

		count--;
	}

	APP_ERROR_CHECK(nrf_drv_spi_transfer(spi_addr, m_tx_buf, m_length, m_rx_buf, m_length));
	
	while (!spi_xfer_done) {
    	__WFE();
    }


    // nrf_gpio_pin_write(MUX_PIN_A, 1);
    // nrf_gpio_pin_write(MUX_PIN_B, 1);
    // rgb_color_t last_led = strips[id][last[id]];
    // rgb_color_t last_led2 = strips[id][(last[id] + 1)%NUM_LEDS];
    // bool last_on = ((last_led.r != 0) || (last_led.g != 0) || (last_led.b != 0));
    // last_on = last_on || ((last_led2.r != 0) || (last_led2.g != 0) || (last_led2.b != 0));
    // printf("last_on: %d\n", last_on);
    return;
}

/*
 * Function that returns true if there is a light on in the hit zone

 * @param 	id 		id of strip to check hit zone for
 */
bool check_hit_zone(int8_t id) {
	bool check = false;
	for (int i = 0; i < HZ_SIZE; i++) {
		check = (check || hit_zone[id][i]);
	}
	return check;
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

/*
 * Function that lights all currently DARK LEDs, dim GREEN
 * Used for successful hit

 * @param 	id 		strip id to light
 */
void green_backlight(int8_t id) {
	for (int i = 0; i < NUM_STRIPS; i++) {
		if (strips[id][i].r != 0 || strips[id][i].g != 0 || strips[id][i].b != 0) {
			continue;
		} else {
			strips[id][i] = (rgb_color_t) {.r = 0, .g = 50, .b = 0};
		}
	}
}

/*
 * Function that lights all currently DARK LEDs, dim RED
 * Used for successful hit

 * @param 	id 		strip id to light
 */
void red_backlight(int8_t id) {
	for (int i = 0; i < NUM_STRIPS; i++) {
		if (strips[id][i].r != 0 || strips[id][i].g != 0 || strips[id][i].b != 0) {
			continue;
		} else {
			strips[id][i] = (rgb_color_t) {.r = 50, .g = 0, .b = 0};
		}
	}
}

/*
 * Function that prints the current state of the LED strips
 */
void printState() {
	for (int i = 0; i < NUM_LEDS; i++) {
		for (int j = 0; j < NUM_STRIPS; j++) {
			printf("| %d ", strips[j][i].r);
			printf("%d ", strips[j][i].g);
			printf("%d |", strips[j][i].b);
		}
		printf("\n");
	}
	printf("\n");
}