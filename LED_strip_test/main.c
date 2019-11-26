// Blink app
//
// Blinks an LED

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include <stdbool.h>
//#include <stdint.h>
//#include "nrf.h"
//#include "nrf_delay.h"
//#include "nrf_gpio.h"

#include "LEDStripAPI.h"

// Pin definitions
//#define LED NRF_GPIO_PIN_MAP(0,17)
#define SPI_INSTANCE  1 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */

/* FOLLOWING BLOCK DOES NOT CALL LED API *******************************************************************

static volatile bool spi_xfer_done;  // < Flag used to indicate that SPI instance completed the transfer.

//#define TEST_STRING "Nordic"
//static const uint8_t leds = 32; 
static const uint8_t m_length = 32*3;        // < Transfer length. 
static uint8_t       m_tx_buf[96];           // < TX buffer. 
static uint8_t       m_rx_buf[96];    		 // < RX buffer.


// @brief SPI user event handler.
// @param event
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}

void setLED(int pos) {
	for (uint8_t i = 0; i < m_length; i+=3) {
	  	if ((i) == pos) {
	  		m_tx_buf[i] = 255;
	  	  	m_tx_buf[i+1] = 255;
	  	  	m_tx_buf[i+2] = 255;
	  	} else {
		  	m_tx_buf[i] = 0;
		  	m_tx_buf[i+1] = 0;
		  	m_tx_buf[i+2] = 0;
	  	}
	}
}

/* END BLOCK **************************************************************************************************/

led_strip_t strip0;

int main(void) {
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	NRF_LOG_DEFAULT_BACKENDS_INIT();

	NRF_LOG_INFO("Begin API Testing.");

	led_spi_init(&spi);
	NRF_LOG_INFO("led_spi_init() Successful.");

	led_strip_init(&strip0, 0);
	NRF_LOG_INFO("LED Strip Initialized.");

	nrf_gpio_cfg_output(3);
	nrf_gpio_cfg_output(4);

	nrf_gpio_pin_clear(3);
	nrf_gpio_pin_clear(4);

	NRF_LOG_INFO("GPIO Initialized.");

	NRF_LOG_INFO("Begin Main Loop.");


	int i = 0;
	//rgb_color_t color_default = {.r = 255, .g = 255, .b = 255};

	while(1) {
		if (i%8 == 0) {
			push_next_light(&strip0, (rgb_color_t) {.b = 255, .g = 0, .r = 0});
		} else if (i%8 == 1) {
			push_next_light(&strip0, (rgb_color_t) {.b = 0, .g = 255, .r = 0});
		} else {
			push_next_light(&strip0, (rgb_color_t) DARK);
		}
		
		show(&strip0);

		// switch stip on mux and show on other strip
		nrf_gpio_pin_toggle(4);

		show(&strip0);

		NRF_LOG_FLUSH();

		if (i++ > NUM_LEDS) {
			i = 0;
		}

		nrf_delay_ms(100);
	}

/* BELOW IS THE WORKING TEST CODE FROM FIRST COMMIT ******************************************************
  //bsp_board_init(BSP_INIT_LEDS);
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // SPI CONFIGURATION 
  // REFERENCE: nrf52x-base/sdk/nrf5_sdk_15.3.0_thread/integration/nrfx/legacy/nrf_drv_spi.h

  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin    = 31;
  spi_config.miso_pin  = 30;
  spi_config.mosi_pin  = 29;
  spi_config.sck_pin   = 28;
  spi_config.frequency = NRF_DRV_SPI_FREQ_125K;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

  NRF_LOG_INFO("SPI example started.");

  for (uint8_t i = 0; i < m_length; i+=3) {
  	// m_tx_buf[i] = 255;
  	// m_tx_buf[i+1] = 255;
  	// m_tx_buf[i+2] = 255;
  	m_tx_buf[i] = 0;
  	m_tx_buf[i+1] = 0;
  	m_tx_buf[i+2] = 0;
  }
  // m_tx_buf[0+94] = 255;
  // m_tx_buf[1+94] = 255;
  // m_tx_buf[2+94] = 255;
  // m_tx_buf[3] = 255;
  // m_tx_buf[4] = 255;
  // m_tx_buf[5] = 255;

  int i = 0;

  while (1)
  {
      // Reset rx buffer and transfer done flag
  	  setLED(i*3);
      memset(m_rx_buf, 0, m_length);
      spi_xfer_done = false;
      APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length));

      while (!spi_xfer_done)
      {
          __WFE();
      }
      printf("first data transfered: %d\n", m_tx_buf[0]);
      NRF_LOG_FLUSH();

      //bsp_board_led_invert(BSP_BOARD_LED_0);
      if (i++ >= 33) {
      	i = 0;
      }
      nrf_delay_ms(100);
  }
  
// Initialize.
//nrf_gpio_cfg_output(LED);

// Enter main loop.
//while (1) {
//    nrf_gpio_pin_toggle(LED);
//    nrf_delay_ms(500);
//  }

/* END BLOCK *********************************************************************************************/


}

