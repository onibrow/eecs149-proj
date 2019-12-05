// SD card app
//
// Writes sample lines to the SD card on Buckler

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
//#include "nrf_drv_spi.h" //-> shouldn't be needed 

#include "buckler.h"
#include "simple_logger.h"


int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // Enable SoftDevice (used to get RTC running)
  nrf_sdh_enable_request();

  // Initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  printf("GPIO initialized ...\n");

  // Configure GPIOs
  nrf_gpio_cfg_output(BUCKLER_SD_ENABLE);
  nrf_gpio_cfg_output(BUCKLER_SD_CS);
  nrf_gpio_cfg_output(BUCKLER_SD_MOSI); // change to lcd mosi later
  nrf_gpio_cfg_output(BUCKLER_SD_SCLK); // change to lcd mosi later
  nrf_gpio_cfg_input(BUCKLER_SD_MISO, NRF_GPIO_PIN_NOPULL); // change to lcd mosi later

  nrf_gpio_pin_set(BUCKLER_SD_ENABLE);
  nrf_gpio_pin_set(BUCKLER_SD_CS);

  printf("SD card initialized!\n");

  // Initialize SD card
  //const char filename[] = "TESTFILE.LOG";
  const char filename[] = "testfile.txt";
  const char permissions[] = "a,r"; // w = write, a = append, r = read
  char read_buff [9];


  printf("before log init \n");

  // Start file
  APP_ERROR_CHECK(simple_logger_init(filename, permissions));

  // If no header, add it
  //simple_logger_log_header("HEADER for file \'%s\', written on %s \n", filename, "DATE");
  //printf("Wrote header to SD card\n");

  // Write sample lines
  int nr_lines = 100;

  printf("before calling logger_log \n");


  /*for (int i = 0; i < nr_lines; i++) {
    printf("Writing to the text file <%d>\n", i);
    simple_logger_log("%d", i);
    // simple_logger_log("%s: Line %i - Additional line added\n", "TIMESTAMP", i+1);
  }*/

  printf("\nbefore calling logger_read \n");
  printf("Reading buffer\n");
  simple_logger_read((uint8_t *)read_buff, 8);
  read_buff[8] = '\0';

  printf("Contents: %s\n\n", read_buff);

/*
  for (int i = 0; i < nr_lines; i++) {
    printf("Reading buffer <%d>\n", i);
    simple_logger_read((uint8_t *)read_buff, 4);

    printf("Contents: %s\n\n", read_buff);
  }
*/
  printf("end of for loop... \n");

  // Signal that lines were written
  nrf_gpio_cfg_output(BUCKLER_LED0);
  nrf_gpio_pin_clear(BUCKLER_LED0);
}
