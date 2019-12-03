// SD card + Display 

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
#include "nrf_drv_clock.h"
#include "nrf_drv_spi.h"
#include "nrf_sdh.h"
#include "nrfx_gpiote.h"

#include "buckler.h"
#include "gpio.h" 
#include "display.h"
#include "simple_logger.h"
#include "app_scheduler.h"


// *****************************************************************
// ************************** VARIABLES ****************************
// *****************************************************************
bool GAMEON = false;
bool gameon_button = false;     // same as gameon, change name later  
bool DONE_PLAYING = false;      // indicator that music is done 
ret_code_t error_code;          // error checking - variously used 
uint16_t score = 0;             // Record the score 
char score_str[16];             // convert score to str for display

////////////////////////////////////////////////////////// Readings
bool a = true;                  // Dummy variable for Demo
uint16_t test_readings[256];    
uint16_t read_idx = 0;          // Indicator which reading to read for buffer
uint16_t buffer_idx = 0;          
uint16_t BUFFER_SIZE = 8;
uint16_t test_reading_buffer[8]; // BUFFER_SIZE

APP_TIMER_DEF(BPM240);          // timer for BPM readings
// APP_TIMER_DEF(SAMPLING);        // timer for song sampling
// ^ We don't need this one, SD lib takes care of it 

// declare variables used for sd card 
const char filename[] = "testfile.txt";
const char permissions[] = "a,r"; // w = write, a = append, r = read
#define READ_BUFFER_SIZE	32 // TODO: what's the maximum?
char read_buff [READ_BUFFER_SIZE + 1];

// *****************************************************************

/* Interrupt Handler & Initiation from lab */
void GPIOTE_IRQHandler(void) {
    NRF_GPIOTE->EVENTS_IN[0] = 0;

    gpio_config(23, 1);
    gpio_clear(23);
    nrf_delay_ms(500);
    gpio_set(23);

    printf("\n========== Hardware Interreupt Getting Called ==========\n");
}

/* Clock setup - code from Nordic forum */

// ignore p_context function, but you can assign a fn and do the work 
// in case of timer timeout. (Most likely we won't use this one)
static void bpm_timer_timeout(void * p_context) {
    UNUSED_VARIABLE(p_context);
    printf("timer timeout ... \n");
}


/* Reads Button Inputs from Bop It and BeatMap from SD Card.
   Compares the values at 4 Hz frequency
   Scores up if two values match */

// TODO: how much will it read? Do some calculation for sampling rate 
static void bpm_read_callback(void * p_context)
    {
    UNUSED_VARIABLE(p_context);
    printf("tick being called ... \n");

    // buffer storing & reading test
    // read 8 data at a time out of 256 for test purpose

    if (test_reading_buffer[buffer_idx] == 1) {
        display_write("GOOD", DISPLAY_LINE_0);
        display_write("+1", DISPLAY_LINE_1);
        score += 1;//a = false;

    } else {
        display_write("MISS T_T", DISPLAY_LINE_0);
        display_write("...", DISPLAY_LINE_1);//a = true;
    }

    // reset if it reaches the end, increment otherwise 
    (buffer_idx == BUFFER_SIZE - 1) ? buffer_idx = 0 : buffer_idx++; 
  }

// *****************************************************************
// ************************* INITIALIZER ***************************
// *****************************************************************

// initialize RTT library
void rtt_init(void) {
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("Log initialized!\n");
}

// Initialize start button and hardware interrupt,  pin #28.
// gpio config done here. Separate to other file later on.
void start_button_init(void) {
    gpio_config(28, 0); // set a button near switch as an starting button, taking an input
    // TODO: set up for the hardware interrupt 

    NRF_GPIOTE->CONFIG[0] |= 0x00021c01;
    NRF_GPIOTE->INTENSET |= 1;
    NVIC_EnableIRQ(GPIOTE_IRQn);

	///may not need this part 
    // software interrupt 
    software_interrupt_init();
    NVIC_EnableIRQ(SWI1_EGU1_IRQn);

    // set the same priority 
    NVIC_SetPriority(GPIOTE_IRQn, 0);
    NVIC_SetPriority(SWI1_EGU1_IRQn, 0);

    printf("start button config initialized!\n");
}

// timer starts along with start with song play.
// I'm not sure what lfclk_request exactly does, it might be need to call 
// for other timer/interrupt. Feel free to take this out and put wherever.
void bpm_timer_init(void) {

    // this function below stops RTC and resumes at the end of the function call 
	error_code = app_timer_init(); // queue size and something else, chose arbitrarilly now 
    APP_ERROR_CHECK(error_code);
    printf("Timer initialized!\n");
    error_code = app_timer_create(&BPM240, APP_TIMER_MODE_REPEATED, 
                                           		bpm_read_callback);
    APP_ERROR_CHECK(error_code);

    // fires in every 250 ms
    error_code = app_timer_start(BPM240, APP_TIMER_TICKS(250), NULL);
    APP_ERROR_CHECK(error_code);
}

// Initialize SD Card 
// the original (from the example) didn't use SPI. 
void gpio_sdcard_init(void) {
  	// Enable SoftDevice (used to get RTC running)
	nrf_sdh_enable_request();

	// Initialize GPIO driver
	if (!nrfx_gpiote_is_init()) {
		error_code = nrfx_gpiote_init();
	}
	APP_ERROR_CHECK(error_code);

	printf("GPIO initialized ...\n");

    nrf_gpio_cfg_output(BUCKLER_SD_ENABLE);
  	nrf_gpio_cfg_output(BUCKLER_SD_CS);
  	nrf_gpio_cfg_output(BUCKLER_SD_MOSI); //share the same LCD ...
  	nrf_gpio_cfg_output(BUCKLER_SD_SCLK); // share the same LCD
  	nrf_gpio_cfg_input(BUCKLER_SD_MISO, NRF_GPIO_PIN_NOPULL);

	nrf_gpio_pin_set(BUCKLER_SD_ENABLE);
  	nrf_gpio_pin_set(BUCKLER_SD_CS);

    printf("SD card initialized!\n");
}

// *****************************************************************
// **************************** MAIN *******************************
// *****************************************************************

int main(void) {
    error_code = NRF_SUCCESS;
    rtt_init();
    start_button_init();

    // Initialize Display 
    nrf_drv_spi_t spi_instance2 = NRF_DRV_SPI_INSTANCE(2);
    nrf_drv_spi_config_t spi_config_lcd = {
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
    
    error_code = nrf_drv_spi_init(&spi_instance2, &spi_config_lcd, NULL, NULL);
    APP_ERROR_CHECK(error_code);
    display_init(&spi_instance2);
    printf("Display initialized!\n");

    display_write("BOP IT REV.DEMO", DISPLAY_LINE_0);
    display_write("PLAY? ->", DISPLAY_LINE_1);

    // whait till hardware interrupt happens
    do {
    	printf("please work\n");
    	__WFI();
    	printf("God responded... \n");

    	gameon_button = true;
    	GAMEON = true;
    } while(!gameon_button);

    // TODO: can't take the file that I created through the SD card
    gpio_sdcard_init();

  	// Start file
  	error_code = simple_logger_init(filename, permissions);
  	printf("error code: %d\n", error_code);
  	APP_ERROR_CHECK(error_code);

  	// read data into buffer
	printf("Reading buffer\n");
	simple_logger_read((uint8_t *)read_buff, READ_BUFFER_SIZE);
	read_buff[READ_BUFFER_SIZE] = '\0';

	printf("Contents: %s\n\n", read_buff);

	printf("end of for loop... \n");

	// Signal that lines were written
	nrf_gpio_cfg_output(BUCKLER_LED0);
	nrf_gpio_pin_clear(BUCKLER_LED0);

    // *****************************************************************
    // ************************** GAME LOOP ****************************
    // *****************************************************************

	// Loop until it receives signals/inputs indicating the end of song
	// TODO: How do we implement the end of song signal? 
	// We could either hard-code by taking the length of the song and 
	// compare that with timer reading. I think that would be the easiest
	while (!DONE_PLAYING) { 

		printf("looping while loop\n");

		// Initiate timers. It is intended to get called only once when
		// the start button is pressed. 
		if (gameon_button) {

			printf("\n*********** GAME ON ***********\n");
			display_write("GAME ON", DISPLAY_LINE_0);
			display_write("(*_*)", DISPLAY_LINE_1);
			nrf_delay_ms(2000);//for demo

			GAMEON = true; // triggers the main game loop
			bpm_timer_init(); // initiate BPM timer (read data at BPM)
			// ^ gets called after sdh initiated, so should be fine 
			gameon_button = false;
		}

    // main game loop (?)
      if (GAMEON) {

        printf("\nbuffer_idx: %d\n", buffer_idx);

          // read again when read_buffer indicates it has reached its maximum
          if (buffer_idx == 0) {
              for (int i = 0; i < 8; i++) {
                  test_reading_buffer[i] = read_buff[BUFFER_SIZE*read_idx + i];

                  printf("i*read_idx + i: %d\n", BUFFER_SIZE*read_idx + i);
                  printf("reading test file, buffer %d\t%d\n", \
                          read_buff[BUFFER_SIZE*read_idx + i], \
                          test_reading_buffer[i]);
              }
              read_idx++; // move it for next
          }

          // for now, stop after 100000 ticks 
          //printf("timer: %d\n", app_timer_cnt_get());
        	if (app_timer_cnt_get() >= 100000) {
        		  DONE_PLAYING = true;
        	}
      }
      // somewhere in the loop will call a function checking if the song is done 
      nrf_delay_ms(250); // should implement the timer 
  }

    nrf_delay_ms(1000);
    
    // Print Score 
    snprintf(score_str, 16, "%d", score);
    display_write("YOUR SCORE: ", DISPLAY_LINE_0);
    display_write(score_str, DISPLAY_LINE_1);

}
