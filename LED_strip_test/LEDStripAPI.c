#include "LEDStripAPI.h"

ret_code_t led_spi_init(nrf_drv_spi_t const * const p_instance,
						nrf_drv_spi_evt_handler_t    handler,
                        void *                       p_context) {
	return nrf_drv_spi_init(p_instance, &standard_config, handler, NULL);
}

ret_code_t show() {
	// nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length)
	return 0;
}