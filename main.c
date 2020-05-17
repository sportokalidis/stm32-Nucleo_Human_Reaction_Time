#include <platform.h>
#include <gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "delay.h"
#include "lcd.h"
#include "switches.h"
#include "leds.h"


int count = 0;
int signal = 0; 

void button_press_isr(int sources) {
	gpio_set(P_DBG_ISR, 1);
if ((sources << GET_PIN_INDEX(P_SW)) & (1 << GET_PIN_INDEX(P_SW))) {
	//count++;
	leds_set(0,0,0);
	signal--;
}
	gpio_set(P_DBG_ISR, 0);
}


int main(void) {
	switches_init();
	leds_init();

	
	srand(time(NULL));
	int sec=0;

	printf("\n Hello World \n");
	
	// Set up debug signals.
	gpio_set_mode(P_DBG_ISR, Output);
	gpio_set_mode(P_DBG_MAIN, Output);
	
	// Set up on-board switch.
	gpio_set_mode(P_SW, PullUp);
	gpio_set_trigger(P_SW, Rising);
	gpio_set_callback(P_SW, button_press_isr);
	
	__enable_irq();
	
	while (1) {
		
		gpio_toggle(P_DBG_MAIN);
		//leds_set(count & 1, count & 2, count & 4);
		
		
		sec = (int) rand() % 9;
		delay_ms(sec*1000);
		
		leds_set(1,0,0);
		signal++;
		
		//while(!switch_get(P_SW)) {
		//	continue;
		//}
		//signal--;
		//printf(" signal: %d \n", signal);
		
		//leds_set(0,0,0);

	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2016*************************************   
