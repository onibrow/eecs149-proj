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
#include "app_button.h"

#include "buckler.h"
#include "gpio.h" 
#include "display.h"
#include "simple_logger.h"
#include "app_scheduler.h"


// *************************************************************************
// ****************************** VARIABLES ********************************
// *************************************************************************
ret_code_t error_code;          		// error checking - variously used 
APP_TIMER_DEF(BPM240);          		// timer for BPM readings

bool gameon_button 			= false;    // same as gameon, change name later  
bool DONE_PLAYING 			= false;    // indicator that music is done 
uint16_t score 				= 0;        // Record the score 
char score_str[16];             		// convert score to str for display

uint16_t read_idx 			= 0;        // Indicator which reading to read for buffer
uint16_t buffer_idx 		= 0;     
#define BUFFER_SIZE 		8     
#define BEATMAP_SIZE		256

char test_reading_buffer	[BUFFER_SIZE][3]; 
char beatmap 				[BEATMAP_SIZE][3];	// [size of the beatmap][three inputs]

// consider merging these to buckler.h 
#define BopIt_BUTTON0 	 	NRF_GPIO_PIN_MAP(0, 11)
#define BopIt_BUTTON1 		NRF_GPIO_PIN_MAP(0, 12)
#define BopIt_BUTTON2 	 	NRF_GPIO_PIN_MAP(0, 13)
#define BopIt_OUTPUT		NRF_GPIO_PIN_MAP(0, 31)

#define SONG_LENGTH_MS		APP_TIMER_TICKS(5000)

// readings 
uint8_t btn[3];

// *************************************************************************


// ignore p_context function, but you can assign a fn and do the work 
// maybe we can utilize it for displaying the scores 
static void bpm_timer_timeout(void * p_context) {
    UNUSED_VARIABLE(p_context);
    printf("timer timeout ... \n");
}

/* Reads Button Inputs from Bop It and BeatMap from SD Card.
   Compares the values at 4 Hz frequency
   Scores up if two values match */

// TODO: how much will it read? Do some calculation for sampling rate 
static void bpm_read_callback(void * p_context){
    UNUSED_VARIABLE(p_context);
    bool display_good = false;
    printf("BPM Read Callback Being Called ... \n");

    printf("INPUT readings:   Button0: %d, Button1: %d, Button2: %d\n", \
    	 	btn[0], btn[1], btn[2]);

    printf("BEATMAP readings: Button0: %d, Button1: %d, Button2: %d\n", \
    	 	beatmap[buffer_idx][0], \
    	 	beatmap[buffer_idx][1], \
    	 	beatmap[buffer_idx][2]);

    if (btn[0] == beatmap[buffer_idx][0]) {
    	display_good = true; 
    	score += 1;
    	printf("btn 0 hit \n");
    }
    if (btn[1] == beatmap[buffer_idx][1]) {
    	display_good = true;  
    	score += 1;
    	printf("btn 1 hit \n");
    }
    if (btn[2] == beatmap[buffer_idx][2]) {
    	display_good = true; 
    	score += 1;
    	printf("btn 2 hit \n");
    }

    // buffer storing & reading test
    // read 8 data at a time out of 256 for test purpose

    if (display_good) {
        display_write("H I T !", DISPLAY_LINE_0);
        printf("H I T !\n");
        // maybe print out the score -> too much function call? 

    } else {
        display_write("MISS T_T", DISPLAY_LINE_0);
        display_write("...", DISPLAY_LINE_1);
        printf("MISS T_T\n");
    }

    // reset if it reaches the end, increment otherwise 
    buffer_idx ++;
}

// simple, test beatmap generator 
void generate_beatmap(void) {

    for (int i = 0; i < BEATMAP_SIZE; i++) {
    	beatmap[i][0] = 1;
    	beatmap[i][1] = 1;
    	beatmap[i][2] = 1;
    }
}

// *************************************************************************
// ***************************** INITIALIZER *******************************
// *************************************************************************

// initialize RTT library
static void rtt_init(void) {
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("Log initialized!\n");
}

static void buttons_interrupt_handler(uint8_t btn_id) {

    if (!gameon_button) {
		printf("\n*********** GAME ON ***********\n");
    	printf("start button pressed  ... \n");
		display_write("GAME ON", DISPLAY_LINE_0);
		display_write("(*_*)", DISPLAY_LINE_1);

        nrf_delay_ms(2000); // give some time to get ready 

    	gameon_button = true;
		bpm_timer_init();


	} else {
		printf("button interrupt getting called ... \n");
		// have to check individual inputs 
		// could be a timer, sync issue later. maybe better to put in callback 
		for (int i = 0; i < 3; i++) {
			btn[i] = app_button_is_pushed(i) || btn[i] ? 1 : 0;
		}
	}
}

static void buttons_init(void) {

  	nrf_gpio_pin_set(BopIt_OUTPUT);
  	nrf_gpio_cfg_output(BopIt_OUTPUT);

    // set the config for the buttons
  	nrf_gpio_cfg_input(BopIt_BUTTON0, NRF_GPIO_PIN_NOPULL);
  	nrf_gpio_cfg_input(BopIt_BUTTON1, NRF_GPIO_PIN_NOPULL);
  	nrf_gpio_cfg_input(BopIt_BUTTON2, NRF_GPIO_PIN_NOPULL);

	static const app_button_cfg_t BUTTONS[] = {
	    {BopIt_BUTTON0, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
	    {BopIt_BUTTON1, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
	    {BopIt_BUTTON2, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
	};

	error_code = app_button_init(BUTTONS, 3, 50);
    APP_ERROR_CHECK(error_code);

	app_button_enable();

  	printf("Bop It Input Buttons Initialized \n");
}

void timer_init(void) {
	// lkclk request 
    error_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(error_code);
    nrf_drv_clock_lfclk_request(NULL);

    // initialize app timer 
	error_code = app_timer_init(); // queue size and something else, chose arbitrarilly now 
    APP_ERROR_CHECK(error_code);
    printf("Timer initialized!\n");
}

// timer starts along with start with song play.
void bpm_timer_init(void) {

    error_code = app_timer_create(&BPM240, APP_TIMER_MODE_REPEATED, bpm_read_callback);
    APP_ERROR_CHECK(error_code);

    // fires in every 250 ms
    error_code = app_timer_start(BPM240, APP_TIMER_TICKS(250), NULL);
    APP_ERROR_CHECK(error_code);
}


// *************************************************************************
// ******************************** MAIN ***********************************
// *************************************************************************

int main(void) {

    error_code = NRF_SUCCESS;
    rtt_init();
    timer_init();
    buttons_init();
    generate_beatmap();
  	
    // Initialize Display ////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////

    // use WFI to wait until interrupt happens 
    do {
    	printf("please work\n");
    	__WFI(); 
    	printf("responded... \n");

    } while(!gameon_button);


    // *************************************************************************
    // ****************************** GAME LOOP ********************************
    // *************************************************************************

	// Loop until it receives signals/inputs indicating the end of song
	// (assigned as tick to trigger timeout)
	while (!DONE_PLAYING) { 



		/////////////////////////////////////////////////////////////////////////////////////////////

	  	// adjust the time along with the length of the song 
		if (app_timer_cnt_get() >= SONG_LENGTH_MS) {

			APP_ERROR_CHECK(app_timer_stop(BPM240));
			DONE_PLAYING = true;
			
	    }

	    // TODO: interrupt to adjust the speed of the timer. Can use a callback function 
		nrf_delay_ms(250); 
    }
    
    // Print Score 
    printf("game is over...\n");
    nrf_delay_ms(1000);

    snprintf(score_str, 16, "%d", score);
    display_write("YOUR SCORE: ", DISPLAY_LINE_0);
    display_write(score_str, DISPLAY_LINE_1);
}
