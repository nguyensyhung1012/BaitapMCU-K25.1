/* Copyright 2023 NXP */
/* License: BSD 3-clause
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h"
#include <stdio.h>

#define PTD0 0		// blue led
#define PTD15 15    //red led
#define PTD16 16    // green led

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

}
