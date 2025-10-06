#include "S32K144.h"
#include "Driver_NVIC.h"
//#include "clock_and_mode.h"
#define BLUELED_PIN 0		// blue led
#define REDLED_PIN 15    //red led
#define GREENLED_PIN 16    // green led


volatile unsigned long delay_second = 384000u; /* 128 kHZ *3 */
volatile uint8_t delay_done = 0;
void toggle_led(uint32_t pin)
{
    IP_PTD->PTOR = (1 << pin);
}

void LPIT0_init(void)
{
	/* 1. Chọn nguồn clock cho LPIT (LPO 128 kHz = option 7) */
	IP_PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(7);
	IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
    /* 2. Enable module */
    IP_LPIT0->MCR = LPIT_MCR_M_CEN_MASK;   // enable module
    /* 3. Nạp giá trị đếm cho 3s */
    IP_LPIT0->TMR[0].TVAL = delay_second;
    /* 4. Cấu hình Channel 0 */
    IP_LPIT0->TMR[0].TCTRL =  0x00000001;
    /* 5. Bật ngắt channel 0 */
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;
    /* 6. Cấu hình NVIC */
    NVIC_ClearPendingIRQ(LPIT0_Ch0_IRQn);
    NVIC_SetPriority(LPIT0_Ch0_IRQn, 2);
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
}

/* ===== ISR cho LPIT0 Channel 0 ===== */
void LPIT0_Ch0_IRQHandler(void)
{
    if (IP_LPIT0->MSR & LPIT_MSR_TIF0_MASK) {
    	 /* Clear cờ timeout */
        IP_LPIT0->MSR = LPIT_MSR_TIF0_MASK;
        /* báo cho hàm delay3s */
        delay_done = 1;
    }
}

void delay3s(void)
{
    delay_done = 0;
    /* Start timer channel 0 */
    IP_LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
    /* Chờ ISR set cờ */
    while (!delay_done);
    /* Dừng timer nếu muốn one-shot */
    IP_LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK;
}

void set_led(uint32_t red, uint32_t green, uint32_t blue)
{
    // Clear tất cả LED trước
    IP_PTD->PSOR = (1 << REDLED_PIN) | (1 << GREENLED_PIN) | (1 << BLUELED_PIN);

    // Nếu tham số nào = 1 thì bật LED đó (logic ngược: bật = ghi 0)
    if (red)   IP_PTD->PCOR = (1 << REDLED_PIN);
    if (green) IP_PTD->PCOR = (1 << GREENLED_PIN);
    if (blue)  IP_PTD->PCOR = (1 << BLUELED_PIN);
}

int main(void)
{
	//SOSC_init_8MHz();
	//SPLL_init_160MHz();
	//NormalRUNmode_80MHz();
	LPIT0_init();
	 IP_PCC -> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock to PORT D */


	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD ->PDDR |= 1<<BLUELED_PIN; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[0] = 0x00000100; /* Port D0: MUX = GPIO */

	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD->PDDR |= 1<<REDLED_PIN; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[15] = 0x00000100; /* Port D0: MUX = GPIO */

	 /* Configure port D0 as GPIO output (LED on EVB) */
	 IP_PTD->PDDR |= 1<<GREENLED_PIN; /* Port D0: Data Direction= output */
	 IP_PORTD->PCR[16] = 0x00000100; /* Port D0: MUX = GPIO */

	 // Clear tất cả LED trước
	 IP_PTD->PSOR = (1 << REDLED_PIN) | (1 << GREENLED_PIN) | (1 << BLUELED_PIN);


	while(1){
		toggle_led(REDLED_PIN);
		delay3s();
		toggle_led(REDLED_PIN);
		delay3s();
		toggle_led(GREENLED_PIN);
		delay3s();
		toggle_led(GREENLED_PIN);
		delay3s();
		toggle_led(BLUELED_PIN);
		delay3s();
		toggle_led(BLUELED_PIN);
		delay3s();

	}

return 0 ;
}
