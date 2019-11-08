// Button and Switch app
//
// Uses a button and a switch to control LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrf_twi_mngr.h"
#include "nrfx_gpiote.h"

#include "buckler.h"
#include "mcp4725.h"


// I2C manager
// NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized\n");

  // initialize MCP4725 driver
  const mcp4725_pins_config_t mcp_pins = {BUCKLER_SENSORS_SCL, BUCKLER_SENSORS_SDA};
  mcp4725_setup(&mcp_pins);
  printf("MCP4725 Initialized!\n");

  // loop forever
  while (1) {
    mcp4725_set_voltage(256, false);
    printf("Writing 256\n");
    nrf_delay_ms(100);
    mcp4725_set_voltage(528, false);
    printf("Writing 528\n");
    nrf_delay_ms(100);
    mcp4725_set_voltage(1023, false);
    printf("Writing 1023\n");
    nrf_delay_ms(100);
  }
}

