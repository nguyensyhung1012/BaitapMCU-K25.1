#include "S32K144.h"

#define PTD0 0		// blue led
#define PTD15 15    //red led
#define PTD16 16    // green led

//delay funtion
void delay_3s() {
    unsigned long i;
    for (i = 0; i < 300000000; i++) {
    }
}

int main(void) {
	 /* Enable clocks to peripherals (PORT modules) */
	 IP_PCC -> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT D */


	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD ->PDDR |= 1<<PTD0; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[0] = 0x00000100; /* Port D0: MUX = GPIO */

	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD->PDDR |= 1<<PTD15; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[15] = 0x00000100; /* Port D0: MUX = GPIO */

	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD->PDDR |= 1<<PTD16; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[16] = 0x00000100; /* Port D0: MUX = GPIO */
	 while(1) {

	 IP_PTD-> PCOR |= 1<<PTD15; /* Clear Output on port D15 (LED on) */
	 delay_3s() ;
	 IP_PTD-> PSOR |= 1<<PTD15; /* Set Output on port D15 (LED off) */
	 delay_3s() ;
	 IP_PTD-> PCOR |= 1<<PTD16; /* Clear Output on port D16 (LED on) */
	 delay_3s() ;
	 IP_PTD-> PSOR |= 1<<PTD16; /* Set Output on port D16 (LED off) */
	 delay_3s() ;
	 IP_PTD-> PCOR |= 1<<PTD0; /* Clear Output on port D0 (LED on) */
	 delay_3s() ;
	 IP_PTD-> PSOR |= 1<<PTD0; /* Set Output on port D0 (LED off) */
	 delay_3s() ;

	 }
return 0 ;
}
