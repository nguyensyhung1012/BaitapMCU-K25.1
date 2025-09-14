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
#include <stdio.h>
#include <stdint.h>


#define GPIOD_Address     (0x400FF0C0u)
#define PORTD_PCR0        (*(uint32_t*)(0x4004C000u))
#define PCC_PORTD 		  (*(uint32_t*)(0X40065130u))
#define PCC_RTC			  (*(uint32_t*)(0x400650F4u))
#define RTC_Address		  (0x4003D000u)

#define PTD0 0 // PortD0 - blue LED bit 0
#define PTD15 15 //PortD15 - Red Led bit 15
#define PTD16 16 //PortD16 - Green Led bit 16

// GPIO register
typedef struct {
	uint32_t PDOR ;
	uint32_t PSOR ;
	uint32_t PCOR ;
	uint32_t PTOR ;
	uint32_t PDIR ;
	uint32_t PDDR ;
	uint32_t PIDR ;
}GPIO_Type ;

GPIO_Type * GPIOD = (GPIO_Type *)GPIOD_Address ; // Tạo thanh ghi GPIOD

void GPIO_Init(){
// bật clock gate control PCC portD
PCC_PORTD |= (1<<30) ;

// bật Pin mux control PORTD
PORTD_PCR0 |= (1<<8) ;

// cấu hình output cho GPIOD - PORTD0
GPIOD -> PDDR |= (1 << PTD0) ; // set bit 0
GPIOD -> PDDR |= (1 << PTD15) ; // set bit 15
GPIOD -> PDDR |= (1 << PTD16) ; // set bit 16

}

void delay_3s() {
    unsigned long i;
    for (i = 0; i < 300000000; i++) {
        // vòng lặp trống

    }
}

int main(void) {
GPIO_Init() ;

	while(1){
	GPIOD -> PCOR |= (1<< PTD15) ; // clear bit output PORTD0 (led red on )
	delay_3s();
	GPIOD -> PSOR |= (1<< PTD15) ; // set bit output PORTD0 (led red off)
	delay_3s();
	GPIOD -> PCOR |= (1<< PTD16) ; // clear bit output PORTD16 (led green on )
	delay_3s();
	GPIOD -> PSOR |= (1<< PTD16) ; // set bit output PORTD0 (led green off)
	delay_3s();
	GPIOD -> PCOR |= (1<< PTD0) ; // clear bit output PORTD0 (led blue on )
	delay_3s();
	GPIOD -> PSOR |= (1<< PTD0) ; // set bit output PORTD0 (led blue off)
	delay_3s();


	}

}
