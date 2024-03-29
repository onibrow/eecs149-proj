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
#include "LEDStripAPI.h"

#include "buckler.h"
#include "gpio.h" 
#include "display.h"
#include "simple_logger.h"
#include "app_scheduler.h"

//#include "beatmaps.c"


// *************************************************************************
// ****************************** VARIABLES ********************************
// *************************************************************************
ret_code_t error_code;          		// error checking - variously used 
APP_TIMER_DEF(BPM240);          		// timer for BPM readings

uint16_t score 				= 0;        // Record the score 
char score_str[16];             		// convert score to str for display

uint16_t read_idx 			= 0;        // Indicator which reading to read for buffer
uint32_t buffer_idx 		= 0;     
#define BUFFER_SIZE 		8     

uint16_t beatmap_size[3] = {898, 410, 812};
uint16_t beatmap_scores[3] = {429, 107, 755};

uint32_t game_start_time;

char test_reading_buffer	[BUFFER_SIZE][3]; 


uint8_t beatmap 			[898] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,

2, 2, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 
2, 2, 0, 2, 0, 0, 4, 0, 2, 2, 0, 2, 0, 4, 2, 0,
2, 2, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 
2, 2, 0, 2, 0, 0, 4, 0, 2, 2, 0, 2, 0, 4, 2, 0,

1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,

2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,
2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0, 2, 0, 4, 0,

2, 2, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 
2, 2, 0, 2, 0, 0, 4, 0, 2, 2, 0, 2, 0, 4, 2, 0,
2, 2, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 
2, 2, 0, 2, 0, 0, 4, 0, 2, 2, 0, 2, 0, 4, 2, 0,

1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,

2, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
2, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,

2, 2, 0, 2, 0, 0, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 
2, 2, 0, 2, 0, 0, 4, 0, 2, 2, 0, 2, 0, 4, 2, 0,

1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,

1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
1, 1, 0, 1, 0, 0, 4, 0, 2, 2, 0, 2, 0, 0, 4, 0,
4, 4, 0, 4, 0, 0, 4, 0, 2, 2, 0, 2, 0, 2, 4, 0,
0, 0, 0, 0
}; // Sugar (3:50 ~ 230s)



uint8_t beatmap1 			[410] = {
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,

1,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0, 4,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0,
4,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0,

1,0,2,0,1,0,2,0, 2,0,2,0,2,0,2,0, 4,0,2,0,4,0,2,0, 2,0,2,0,2,0,2,0, 1,0,2,0,1,0,2,0, 2,0,2,0,2,0,2,0,
4,0,2,0,4,0,2,0, 2,0,2,0,2,0,2,0, 1,0,0,0,0,0,0,0,

1,0,2,0,1,0,2,0, 2,0,2,0,2,0,2,0, 4,0,2,0,4,0,2,0, 2,0,2,0,2,0,2,0,
2,0,0,0,0,4,0,0, 2,0,2,0,0,4,0,0, 1,0,4,0,4,0,1,0, 2,0,0,0,0,0,0,0, 4,0,0,0,0,0,0,0,
2,0,0,0,4,0,0,0, 2,0,0,0,1,0,0,0, 2,0,0,0,4,0,0,0, 2,0,0,0,1,0,0,0, 2,0,0,0,4,0,0,0, 1,0,0,0,2,0,0,0,
0,0,0,0,1,0,0,0, 0,0,0,0,2,0,0,0, 0,0,0,0,4,0,0,0, 0,0,0,0,2,0,0,0, 0,0,0,0,1,0,0,0,
0,0,0,0,2,0,4,0, 2,0,4,0,2,0,1,0, 2,0,1,0,2,0,4,0, 2,0,4,0,2,0,1,0, 2,0,1,0,2,0,0,0, 4,0,0,0,1,0,0,0, 2,
0,0,0,0
}; // Derezzed (1:44 ~ 104s)


uint8_t beatmap2 			[808] = {
0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,
3,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0, 6,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0,
3,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0, 6,0,4,2,2,4,0,0, 2,0,4,2,2,4,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
2,0,0,0,0,0,0,0,

0,0,2,0,2,0,1,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0,
2,0,2,0,2,0,1,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0,

0,0,4,0,4,0,2,0, 1,0,0,0,0,0,0,0, 2,0,0,0,4,0,0,0, 2,0,0,0,0,0,0,0,
6,0,0,0,3,0,0,0,

6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,

2,0,0,0,0,0,0,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0,
2,0,2,0,2,0,1,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0, 2,0,4,2,2,4,0,0,

0,0,4,0,4,0,2,0, 1,0,0,0,0,0,0,0, 2,0,0,0,4,0,0,0, 2,0,0,0,0,0,0,0,
6,0,0,0,3,0,0,0,

6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 1,1,1,1,3,3,3,3,
2,2,2,2,2,1,2,1,

2,0,2,0,2,0,6,0, 4,0,0,0,0,1,2,1, 2,0,2,0,2,0,4,0, 6,0,0,0,4,0,1,0,
2,0,2,0,2,0,6,0, 4,0,0,0,0,1,2,1, 2,0,2,0,2,0,4,0, 6,0,2,0,1,0,2,0,
4,0,2,0,1,0,2,0,

4,0,0,0,0,0,0,0, 4,0,2,0,1,0,0,0, 0,0,4,0,4,0,2,0, 6,0,0,0,2,0,0,0,
6,0,0,0,0,0,0,0, 4,0,2,0,1,0,0,0, 0,0,4,0,4,0,2,0, 6,0,0,0,0,0,0,0,
3,0,0,0,0,0,0,0,

6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,1,1,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,3,3,2,2,
6,6,6,6,6,6,2,2, 6,6,6,6,6,6,4,4, 6,6,6,6,6,6,2,2, 3,3,3,3,1,1,2,2,
4,0,0,0,2,0,1,0, 2,0,0,0,2,0,4,0, 1,0,0,0,2,0,1,0, 2,0,0,0,1,0,2,0,
0,0,0,0
}; // What I've Done (3:25 ~ 205s)

// song lengths 
uint32_t timeout;
uint32_t   timeouts[3] = { 240000, 110000, 210000 };
uint8_t * beatmap_list[3] = { beatmap, beatmap1, beatmap2 };
uint8_t * beatmap_to_play;

#define BopIt_BUTTON0 	 	NRF_GPIO_PIN_MAP(0, 11)
#define BopIt_BUTTON1 		NRF_GPIO_PIN_MAP(0, 12)
#define BopIt_BUTTON2 	 	NRF_GPIO_PIN_MAP(0, 13)

#define BopIt_OUTPUT		NRF_GPIO_PIN_MAP(0, 14)
#define BopIt_OUTPUT2		NRF_GPIO_PIN_MAP(0, 19)

#define LED_GREEN			NRF_GPIO_PIN_MAP(0, 20)
#define LED_RED				NRF_GPIO_PIN_MAP(0, 22)

#define SONG_LENGTH_MS		APP_TIMER_TICKS(240000)

typedef enum {
	NOPLAY,
	PLAY,
	GAMEOVER
} states_t;

states_t game;

uint8_t btn[3];

uint8_t onbeat;

rgb_color_t bop_it_colors[3] = {
	(rgb_color_t) {.r = 255, .g = 195, .b = 5},
	(rgb_color_t) {.r = 255, .g = 32,  .b = 0},
	(rgb_color_t) {.r = 0,   .g = 255, .b = 255}};

char song_titles[3][17] = {
	"<    SUGAR    >", "<  DEREZZED   >", "<WHAT IVE DONE>"
};

uint8_t song_select = 0;

uint8_t beat_passed[3]; // beat is present (1), missing (0), passed (2), or hit (3)



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
    if (game != PLAY) {
    	return;
    }

    bool hit_good = false;
    int8_t which_hit[3] = {-1,-1,-1};
    if (buffer_idx >= beatmap_size[song_select]) {

    	push_next_light(0, (rgb_color_t) DARK);
    	push_next_light(1, (rgb_color_t) DARK);
    	push_next_light(2, (rgb_color_t) DARK);
    	show(0);
    	show(1);
    	show(2);
    	return;
    }
    // printf("BPM Read Callback Being Called ... \n");
    if (onbeat == 0) {
    printf("\n\nINPUT:   Button0: %d, Button1: %d, Button2: %d\n", \
    	 	btn[0], btn[1], btn[2]);

    // printf("BEATMAP: Button0: %d, Button1: %d, Button2: %d\n", \
    	 	beatmap[buffer_idx][0], beatmap[buffer_idx][1], beatmap[buffer_idx][2]);
	}
	bool last_on = false;

    for (int i = 0; i < 3; i++) {
    	if (((beatmap_to_play[buffer_idx] >> (2-i)) % 2) == 1 && onbeat == 0) {
    		push_next_light(i, bop_it_colors[i]);
    	} else {
    		push_next_light(i, (rgb_color_t) DARK);
    	}
    	last_on = check_hit_zone(i);

    	// show(i);

    	if (last_on) {
    		which_hit[i] = 1;
    		if (beat_passed[i] == 0) {
    			beat_passed[i] = 1;
    		}
    	} else {
    		if (beat_passed[i] == 1) {
    			beat_passed[i] = 2;
    		} else {
    			beat_passed[i] = 0;
    		}
    	}

    	hit_good = ((last_on && (btn[i] == 1)) || hit_good);

    }

	nrf_gpio_pin_write(LED_GREEN, 0);
	nrf_gpio_pin_write(LED_RED, 0);

    printf("BEAT_PASSED: Strip %d    |     %d    |    %d\n", beat_passed[0], beat_passed[1], beat_passed[2]);
    // printf("LAST LED ON: %d\n", hit_good);
    if (hit_good) {
        display_write("H I T !", DISPLAY_LINE_0);
        for (uint8_t i = 0; i < 3; i++) {
        	if (which_hit[i] && btn[i] && beat_passed[i] == 1) {
        		green_backlight(i);
        		score++;
        		beat_passed[i] = 3;
        	}
        }

        printf("H I T !\n");
		// nrf_gpio_pin_write(LED_GREEN, 1);

    } else if (beat_passed[0] == 2 || beat_passed[1] == 2 || beat_passed[2] == 2){
    	printf("MISSED...\n");
    	display_write("MISSED...", DISPLAY_LINE_0);
    	display_write("", DISPLAY_LINE_1);
    	beat_passed[0] %= 2;
    	beat_passed[1] %= 2;
    	beat_passed[2] %= 2;

		// nrf_gpio_pin_write(LED_RED, 1);
    } else {
        display_write("~~~", DISPLAY_LINE_0);
        display_write("", DISPLAY_LINE_1);
        printf("~~~\n\n");
    }

    show(0);
    show(1);
    show(2);

    if (onbeat == 0) {
	    buffer_idx ++;
    }
    // printf("last_on: %d\n\n", last_on);

    btn[0] = 0;
    btn[1] = 0;
    btn[2] = 0;
    onbeat++;
    onbeat %= 4;
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

	switch (game) {
	    case NOPLAY: {
			printf("\n*********** GAME ON ***********\n");
	    	printf("start button pressed  ... \n");

			for (int i = 0; i < 3; i++) {
				btn[i] = app_button_is_pushed(i);
			}

			if (btn[1] == 1) {
				beatmap_to_play = beatmap_list[song_select];
	    		game = PLAY;

	    		if (song_select == 0) {
			        nrf_gpio_pin_write(BopIt_OUTPUT, 1);
			        nrf_gpio_pin_write(BopIt_OUTPUT2, 0);
			        timeout = timeouts[0];
			        nrf_delay_ms(150);
	    		} else if (song_select == 1) {
			        nrf_gpio_pin_write(BopIt_OUTPUT, 0);
			        nrf_gpio_pin_write(BopIt_OUTPUT2, 1);
			        timeout = timeouts[1];
			        nrf_delay_ms(60);
	    		} else if (song_select == 2) {
			        nrf_gpio_pin_write(BopIt_OUTPUT, 1);
			        nrf_gpio_pin_write(BopIt_OUTPUT2, 1);
			        timeout = timeouts[2];
			        nrf_delay_ms(158);
	    		}

		        nrf_delay_ms(250); 

			    nrf_gpio_pin_write(BopIt_OUTPUT, 0);
			    nrf_gpio_pin_write(BopIt_OUTPUT2, 0);

		    	game = PLAY;
				bpm_timer_init();
				game_start_time = app_timer_cnt_get();
				break;
			}

			if (btn[0] == 1) {
				if (song_select == 0) {
					song_select = 2;
				} else {
					song_select--;
				}
			}

			if (btn[2] == 1) {
				if (song_select == 2) {
					song_select = 0;
				} else {
					song_select++;
				}
			}

			char display_title[16];
			snprintf(display_title, 16, "%s", song_titles[song_select]);
			display_write("Bop It To Play", DISPLAY_LINE_0);
		    display_write(display_title, DISPLAY_LINE_1);
			break;
		} 
		case PLAY: {
			for (int i = 0; i < 3; i++) {
				btn[i] = app_button_is_pushed(i) || btn[i] ? 1 : 0;
			}
			break;
		}
	}
}

static void buttons_init(void) {

  	nrf_gpio_pin_set(BopIt_OUTPUT);
  	nrf_gpio_cfg_output(BopIt_OUTPUT);
    nrf_gpio_pin_write(BopIt_OUTPUT, 0);

  	nrf_gpio_pin_set(BopIt_OUTPUT2);
  	nrf_gpio_cfg_output(BopIt_OUTPUT2);
    nrf_gpio_pin_write(BopIt_OUTPUT2, 0);

  	nrf_gpio_pin_set(LED_GREEN);
  	nrf_gpio_cfg_output(LED_GREEN);
    nrf_gpio_pin_write(LED_GREEN, 0);

  	nrf_gpio_pin_set(LED_RED);
  	nrf_gpio_cfg_output(LED_RED);
    nrf_gpio_pin_write(LED_RED, 0);

    // set the config for the buttons
  	nrf_gpio_cfg_input(BopIt_BUTTON0, NRF_GPIO_PIN_NOPULL);
  	nrf_gpio_cfg_input(BopIt_BUTTON1, NRF_GPIO_PIN_NOPULL);
  	nrf_gpio_cfg_input(BopIt_BUTTON2, NRF_GPIO_PIN_NOPULL);


	static const app_button_cfg_t BUTTONS[] = {
	    {BopIt_BUTTON0, APP_BUTTON_PUSH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
	    {BopIt_BUTTON1, APP_BUTTON_PUSH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
	    {BopIt_BUTTON2, APP_BUTTON_PUSH, NRF_GPIO_PIN_NOPULL, buttons_interrupt_handler},
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
	error_code = app_timer_init(); 
    APP_ERROR_CHECK(error_code);
    printf("Timer initialized!\n");
}

// timer starts along with start with song play.
void bpm_timer_init(void) {

    error_code = app_timer_create(&BPM240, APP_TIMER_MODE_REPEATED, bpm_read_callback);
    APP_ERROR_CHECK(error_code);

    // fires in every 250/4 ms
    error_code = app_timer_start(BPM240, APP_TIMER_TICKS(62.5), NULL);
    APP_ERROR_CHECK(error_code);

    // game_start_time = app_timer_cnt_get();
}


// *************************************************************************
// ******************************** MAIN ***********************************
// *************************************************************************

int main(void) {
	game = NOPLAY;

    error_code = NRF_SUCCESS;
    rtt_init();
    timer_init();
    buttons_init();
    // generate_beatmap();

	nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(1); 
	led_spi_init(&spi);
	led_strips_init();
  	
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

    

    display_write("BOP IT REV.", DISPLAY_LINE_0);
    display_write("LOADING...", DISPLAY_LINE_1);

    nrf_delay_ms(5000); 

	display_write("Bop It To Play", DISPLAY_LINE_0);
	display_write("<    SUGAR    >", DISPLAY_LINE_1);

    onbeat = 1;
    beat_passed[0] = 0;
    beat_passed[1] = 0;
    beat_passed[2] = 0;

    uint32_t cur_time = 0;
    uint32_t time_diff = 0;


    //////////////////////////////////////////////////////////////////////

    while (true) {
	    switch (game) {
	    	case NOPLAY: {
	    		__WFI(); 
	    		break;
	    	}
	    
			case PLAY: { 
				cur_time = app_timer_cnt_get();
				// printf("cur_time: %d  |  SONG LENGTH: %d\n", cur_time, SONG_LENGTH_MS);
				// printf("difference: %d\n\n", app_timer_cnt_diff_compute(cur_time, game_start_time));
				time_diff = app_timer_cnt_diff_compute(cur_time, game_start_time);

				if (time_diff >= APP_TIMER_TICKS(timeout)) {

					APP_ERROR_CHECK(app_timer_stop(BPM240));
					game = GAMEOVER;
					clear_led_strip(0);
    				clear_led_strip(1);
    				clear_led_strip(2);
    				show(0);
    				show(1);
    				show(2);
			    }
			    break;
			}

			case GAMEOVER: {
				// Print Score 
			    printf("game is over...\n");

			    snprintf(score_str, 16, "SCORE: %d/%d", score, beatmap_scores[song_select]);
			    display_write(score_str, DISPLAY_LINE_0);
			    display_write("PLAY AGAIN? ->", DISPLAY_LINE_1);

			    nrf_delay_ms(2000);
			    buffer_idx = 0;
			    score = 0;
			    song_select = 0;
			    game = NOPLAY;
			    break;
			}
		}
    }

}
