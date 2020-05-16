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

void print_switches() {
	lcd_set_cursor(0, 1);
	
	if (switch_get(P_SW_UP)) {
		printf("\n     UP     \n"  );
	} else if (switch_get(P_SW_CR)) {
		printf("\n     CENTER     \n"  );
	} else if (switch_get(P_SW_DN)) {
		printf("\n     DOWN     \n"  );
	} else if (switch_get(P_SW_LT)) {
		printf("\n     LEFT     \n"  );
	} else if (switch_get(P_SW_RT)) {
		printf("\n     RIGHT     \n"  );
	} else {
		printf("\n     NOTHING     \n"  );
	}
}

void light_leds() {
	leds_set(switch_get(P_SW),
	         switch_get(P_SW_CR),
	         switch_get(P_SW_DN));
}

int signal = 0; 

int main(void) {
	switches_init();
	leds_init();

	
	srand(time(NULL));
	//int sw = -1;
	int sec=0;

	printf("\n Hello World \n");
	
	while (1) {
		// print_switches();
	  // light_leds();
		
		//sw = switch_get(PC_13);
		//printf("sw: %d \n", sw);
		
		//if(sw == 1) {
		//	leds_set(1,0,0);
	  // } 
		
		//sec = (int) rand() % 9;
		//sec = sec * 1000;
		//printf("%d \n", sec);
		
		// delay_ms(sec);
		// leds_set(1, 0, 0);
		
		sec = (int) rand() % 9;
		//sec = 1;
		delay_ms(sec*1000);
		
		leds_set(1,0,0);
		signal++;

		while(!switch_get(PC_13)) {
			continue;
		}
		signal--;
		printf(" signal: %d \n", signal);
		
		leds_set(0,0,0);

	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2016*************************************   
