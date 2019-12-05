#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "nrf_delay.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "buckler.h"
#include "simple_logger.h"

int main(void) {
  // RTT
  ret_code_t error_code = NRF_SUCCESS;
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  nrf_sdh_enable_request();

  // Initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  // Configure GPIOs
  nrf_gpio_cfg_output(BUCKLER_SD_ENABLE);
  nrf_gpio_cfg_output(BUCKLER_SD_CS);
  nrf_gpio_cfg_output(BUCKLER_SD_MOSI); // change to lcd mosi later
  nrf_gpio_cfg_output(BUCKLER_SD_SCLK); // change to lcd mosi later
  nrf_gpio_cfg_input(BUCKLER_SD_MISO, NRF_GPIO_PIN_NOPULL); // change to lcd mosi later

  nrf_gpio_pin_set(BUCKLER_SD_ENABLE);
  nrf_gpio_pin_set(BUCKLER_SD_CS);

  printf("\n====================\n");
  printf("SD card initialized!\n");
  printf("====================\n\n");

  // Initialize SD card
  const char filename[] = "testfile.txt";
  const char permissions[] = "a,r"; // w = write, a = append, r = read (?)
  uint8_t BUF_SIZE = 8;
  char read_buff [BUF_SIZE + 1];

  // Start file
  APP_ERROR_CHECK(simple_logger_init(filename, permissions));

  simple_logger_reset_fp();
  read_buff[BUF_SIZE] = '\0';

  printf("File Contents: \n\n");
  bool file_done = false;
  while (!file_done) {
    file_done = simple_logger_read((uint8_t *) read_buff, BUF_SIZE) != 0;
    printf("%s", read_buff);
    nrf_delay_ms(1);
  }
  printf("\n\nDone reading.\n");

  // Signal that lines were written
  nrf_gpio_cfg_output(BUCKLER_LED0);
  nrf_gpio_pin_clear(BUCKLER_LED0);
}
