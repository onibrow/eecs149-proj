#include "gpio.h"

#define BASE_ADDR  			0x50000504
gpio_t* gpio = (gpio_t*) BASE_ADDR;

// Inputs: 
//  gpio_num - gpio number 0-31
//  dir - gpio direction (INPUT, OUTPUT)
void gpio_config(uint8_t gpio_num, gpio_direction_t dir) {

	if (dir == 0) {
		gpio->DIR &= ~(1 << gpio_num);// note: we might not need this part, we can control by changing cnf to 2 instead of 1. 
		                              // let's read documentation more carefully ;( )
		gpio->cnf[gpio_num] &= ~(1 << 1);
		// change the direction to be input (0) 
	} else {

		gpio->DIR |= (1 << gpio_num);
	}

	printf("\nconfig # %d is being called...\n", gpio_num);

}

// Set gpio_num high
// Inputs: 
//  gpio_num - gpio number 0-31
void gpio_set(uint8_t gpio_num) {
	// why we have to use OUT for set? => bc we WRITE 
	gpio->OUT |= (1 << gpio_num);

	printf("\nset # %d is being called...\n", gpio_num);
}

// Set gpio_num low
// Inputs: 
//  gpio_num - gpio number 0-31
void gpio_clear(uint8_t gpio_num) {
	gpio->OUT &= ~(1 << gpio_num);
	// Q. Question: why don't we use OUTCLR instead?
}

// Inputs: 
//  gpio_num - gpio number 0-31
bool gpio_read(uint8_t gpio_num) {
    // should return pin state
	printf("\nin # %d is being called...\n", gpio_num);
	
    return ((gpio->IN & (1 << gpio_num)) >> gpio_num);
    //((*GPIO_in >> (1 << gpio_num)) & 01) >> gpio_num; // see if it works?
}