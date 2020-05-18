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

#define LOOP 5 
#define MOD 1

#define  ARM_CM_DEMCR      (*(uint32_t *)0xE000EDFC)
#define  ARM_CM_DWT_CTRL   (*(uint32_t *)0xE0001000)
#define  ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)

void init_timer(){
	if (ARM_CM_DWT_CTRL != 0) {        // See if DWT is available

			ARM_CM_DEMCR      |= 1 << 24;  // Set bit 24

			ARM_CM_DWT_CYCCNT  = 0;

			ARM_CM_DWT_CTRL   |= 1 << 0;   // Set bit 0
	}
}

int count = 0;
int signal = 0; 

uint32_t start;
uint32_t end;
uint32_t total_cycles;
uint32_t total_time;

uint32_t sum = 0; 

void button_press_isr(int sources) {
	end  = ARM_CM_DWT_CYCCNT;
	total_cycles = end - start;
	// """ Stop measure clock cycles
	printf(" total_cycles: %zu\n", total_cycles);
	
	total_time = total_cycles / (SystemCoreClock * 1e-3);
	printf(" total_time: %zu\n\n", total_time);
	
	sum += total_time;
	
	gpio_set(P_DBG_ISR, 1);
	if ((sources << GET_PIN_INDEX(P_SW)) & (1 << GET_PIN_INDEX(P_SW))) {
		count++;
		leds_set(MOD,0,0);
		signal = 0;
	}
	gpio_set(P_DBG_ISR, 0);
}


int main(void) {
	switches_init();
	leds_init();
	leds_set(MOD,0,0);
	
	init_timer();

	float average_time = 0;
	
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
	
	__disable_irq();
	
	while (1) {
		
		gpio_toggle(P_DBG_MAIN);
		//leds_set(count & 1, count & 2, count & 4);
		
		if(signal == 0 && count < LOOP) {
			__disable_irq();
			
			sec = (int) rand() % 9;
			delay_ms(sec*1000);
		
			// int result = (MOD+1)%2;
			leds_set((MOD+1)%2,0,0);
			
			// """  start to measure clocks here """
			start = ARM_CM_DWT_CYCCNT; 
			
			signal = 1;
			
			__enable_irq();
		}
		
		if(count == LOOP) {
			average_time = ((float) sum)/ LOOP;
			printf(" average time = %f\n", average_time);
			
			count=0;
		}
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2016*************************************   
