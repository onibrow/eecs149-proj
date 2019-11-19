// Blink app
//
// Blinks the LEDs on Buckler

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrf_drv_clock.h" //in legacy 

#include "buckler.h"
#include "gpio.h"
#include "display.h"

bool interrupt;
bool GAMEON = false;
bool gameon_button = false; // same as gameon, change its name later 
bool DONE_PLAYING = false;
ret_code_t error_code;
uint16_t score = 0;

bool a = true; //for demo

APP_TIMER_DEF(BPM240); // 4 Hz

void SWI1_EGU1_IRQHandler(void) {
    NRF_EGU1->EVENTS_TRIGGERED[0] = 0;
    // NRF_GPIOTE->EVENTS_IN[0] = 0; // from hardware IRQHandler 
    printf("\n*********** GAME ON ***********\n");
      display_write("GAME ON", DISPLAY_LINE_0);
  	  display_write("(*_*)", DISPLAY_LINE_1);
  	  nrf_delay_ms(2000);//for demo
}

void software_interrupt_init(void) {
    NRF_EGU1->INTENSET = 0x1;
    NVIC_EnableIRQ(SWI1_EGU1_IRQn);
}

void software_interrupt_generate(void) {
    NRF_EGU1->TASKS_TRIGGER[0] = 1;
}

static void lfclk_request(void)
{
    error_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(error_code);
    nrf_drv_clock_lfclk_request(NULL);
}

// need to work on this  ... :o 
static void bpm_timer_tick_callback(void * p_context)
{

    //UNUSED_VARIABLE(p_context);
	printf("tick being called ... \n");
    //uint32_t error_code = iot_timer_update();
    //APP_ERROR_CHECK(error_code);
    // read_bopit_input();
    // compare the value with song (in buffer -> by how much?)
    //if (song_buf[i] == read_input) {
	if (a) {
		display_write("GOOD", DISPLAY_LINE_0);
		display_write("+1", DISPLAY_LINE_1);
		score += 1;
		a = false;
	} else {
		display_write("MISS T_T", DISPLAY_LINE_0);
		display_write("...", DISPLAY_LINE_1);
		a = true;
	}
}

static void bpm_timer_timeout(void * p_context)
{
	printf("timer timerout ... \n");
}

////////////////////////////////////////////////////////////////////
////////////////////////// INITIALIZERS ////////////////////////////
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



int main(void) {

  error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");
  // ----------------------------------------------------------------
  gpio_config(28, 0); //button 

  NRF_GPIOTE->CONFIG[0] |= 0x00021c01;
  NRF_GPIOTE->INTENSET |= 1;
  NVIC_EnableIRQ(GPIOTE_IRQn);

  // software interrupt 
  software_interrupt_init();
  NVIC_EnableIRQ(SWI1_EGU1_IRQn);

  // set the same priority 
  NVIC_SetPriority(GPIOTE_IRQn, 0);
  NVIC_SetPriority(SWI1_EGU1_IRQn, 0);
  // ----------------------------------------------------------------
  nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };
  
  error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);

  display_write("BOP IT REV.DEMO", DISPLAY_LINE_0);
  display_write("PLAY? ->", DISPLAY_LINE_1);
  printf("Display initialized!\n");
  // ----------------------------------------------------------------

  /* Start when button is pressed (longer than 0.5s pressed)
   * button start -> timer start, song play, start reading the data, ... */

  // ******************************************************
  // ******************** GAME LOOP ***********************
  // ******************************************************

  //int i = 0;
  uint16_t read_input;
  char test[16];


  do { //check the function 
  	  printf("waiting for the button input...\n");
  	  __WFI();
  	  //gameon_button = gpio_read(28);
  	  //printf("%d\n", gameon_button);
  	  printf("received the input...\n");
  	  gameon_button = true;

  } while (!gameon_button);



  while (!DONE_PLAYING) {
    printf("looping while loop\n");

    //__WFI(); // wait until interrupt happens 

    if (gameon_button) { 
      // for some reason, waiting is not happening here. 
      software_interrupt_generate();

  	  GAMEON = true;

  lfclk_request();

  error_code = app_timer_init();
  APP_ERROR_CHECK(error_code);
  printf("Timer initialized!\n");
  error_code = app_timer_create(&BPM240, APP_TIMER_MODE_REPEATED, 
  						    			 bpm_timer_tick_callback);
  APP_ERROR_CHECK(error_code);


  		// learn how tick works 
      error_code = app_timer_start(BPM240, APP_TIMER_TICKS(250), NULL); //APP_TIMER_TICKS(250), &timeout
      APP_ERROR_CHECK(error_code);

      gameon_button = false;
    }


    // game loop 
    // read data in every 0.25 sec 
    if (GAMEON) {
      /*display_write("GAME ON...?", DISPLAY_LINE_0);
      snprintf(test, 16, "%d", i);
      display_write(test, DISPLAY_LINE_1);
      */printf("timer: %d\n", app_timer_cnt_get());
    	if (app_timer_cnt_get() >= 100000) {
    		DONE_PLAYING = true;
    	}

    }


    // somewhere in the loop will call a function checking if the song is done 
    nrf_delay_ms(250); // should implement the timer 
  }


  // print the value
  nrf_delay_ms(1000);

  char score_str[16];
  snprintf(score_str, 16, "%d", score);

  display_write("YOUR SCORE: ", DISPLAY_LINE_0);
  display_write(score_str, DISPLAY_LINE_1);

}