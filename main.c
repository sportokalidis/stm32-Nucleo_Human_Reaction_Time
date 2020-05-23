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


#define LOOP 5    // Number of experiments
#define MOD 0     // The kind of experiment -> 0: The LED off
                  //                        -> 1: The LED on

// DEF to calculate number of clock cycles
#define  ARM_CM_DEMCR      (*(uint32_t *)0xE000EDFC)
#define  ARM_CM_DWT_CTRL   (*(uint32_t *)0xE0001000)
#define  ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)

// Enable and Initialize counter that counts CPU clock cycles
void init_timer(){
	if (ARM_CM_DWT_CTRL != 0) {      // See if DWT is available
		ARM_CM_DEMCR      |= 1 << 24;  // Set bit 24

		ARM_CM_DWT_CYCCNT = 0;

		ARM_CM_DWT_CTRL   |= 1 << 0;   // Set bit 0
	}
}


int count = 0;           // Count the experiments
int signal = 0;          // 0: the led is off , 1: the LED is on, [ Logic Analyzer Variable ]

uint32_t start;          // Number of CPU clock cycles when the LED on
uint32_t end;            // Number of CPU clock cycles when the LED off
uint32_t total_cycles;   // Total number of CPU clock cycles [ end - start]
uint32_t total_time;     // Human reaction time: total_cycle / CPU_freq [in ms]

uint32_t sum = 0;        // sum of all total_times

// Interrupt function
void button_press_isr(int sources) {
	gpio_set(P_DBG_ISR, 1);

	if ((sources << GET_PIN_INDEX(P_SW)) & (1 << GET_PIN_INDEX(P_SW))) {
		end  = ARM_CM_DWT_CYCCNT;      // Take the cpu cycles when an interrupt occur

		total_cycles = end - start;    // Calculate the total num of cycles
		printf(" total_cycles: %zu\n", total_cycles);

		total_time = total_cycles / (SystemCoreClock * 1e-3); // Calculate the total time in ms
		printf(" total_time: %zu\n\n", total_time);

		sum += total_time;             // Sum the total time to find the average at the end

		count++;                       // increase the counter of experiments
		leds_set(MOD,0,0);             // toggle the led
		signal = 0;
	}
	gpio_set(P_DBG_ISR, 0);
}


int main(void) {
	switches_init();
	leds_init();
	leds_set(MOD,0,0);         // Init the LED MOD==0 -> off,   MOD==1 -> on

	init_timer();              // Initialize counter that counts CPU clock cycles

	float average_time = 0;    // The average time of all experiments that take place

	srand(time(NULL));
	int sec=0;

	// Set up debug signals.
	gpio_set_mode(P_DBG_ISR, Output);
	gpio_set_mode(P_DBG_MAIN, Output);

	// Set up on-board switch.
	gpio_set_mode(P_SW, PullUp);
	gpio_set_trigger(P_SW, Rising);
	gpio_set_callback(P_SW, button_press_isr);

  // Disable the interrupts
	__disable_irq();

	while (1) {

		gpio_toggle(P_DBG_MAIN);
		//leds_set(count & 1, count & 2, count & 4);


		if(signal == 0 && count < LOOP) {
			__disable_irq(); // Disable the interrupts

			// delay for a random time between 0 and 8 secs
			sec = (int) rand() % 9;
			delay_ms(sec*1000);

			leds_set((MOD+1)%2,0,0); // MOD==0 -> led on,   MOD==1 -> led off

			// """  start to measure clocks here """
			start = ARM_CM_DWT_CYCCNT;    // take the num of clock cycles
			signal = 1;                   // signal == 1 until the an interrupt occur

			__enable_irq(); // enable the interrupts
		}

		// calculate the average time
		if(count == LOOP) {
			average_time = ((float) sum)/ LOOP;
			printf("** average time = %f **\n\n", average_time);

			sum = 0;
			count=0;
		}
	}
}

// *******************************ARM University Program Copyright � ARM Ltd 2016*************************************
