
#include <stdint.h>

// Base address
#define PCC_PORTD_ADDR   (0x40065130u)
#define PORTD_BASE       (0x4004C000u)
#define GPIOD_BASE       (0x400FF0C0u)

// PCC PORTD
#define PCC_PORTD        (*(uint32_t*)(PCC_PORTD_ADDR))

// PCR register (Pin Control Register) - mỗi PCR chiếm 4 byte
#define PORTD_PCR(n)     (*(uint32_t*)(PORTD_BASE + (n * 4)))

// GPIO register map
typedef struct {
    uint32_t PDOR; // Data Output Register
    uint32_t PSOR; // Set Output Register
    uint32_t PCOR; // Clear Output Register
    uint32_t PTOR; // Toggle Output Register
    uint32_t PDIR; // Data Input Register
    uint32_t PDDR; // Data Direction Register
    uint32_t PIDR; // Pad Data Input Register
} GPIO_Type;

//GPIOD register
#define GPIOD ((GPIO_Type*) GPIOD_BASE)

// LED pins
#define PTD0   0   // Blue LED
#define PTD15  15  // Red LED
#define PTD16  16  // Green LED

// khởi tạo GPIO
void GPIO_Init(void) {
    // Enable clock for PortD
    PCC_PORTD = (1 << 30);

    // Set Pin Mux = ALT1 (GPIO)
    PORTD_PCR(PTD0)  = (1 << 8);
    PORTD_PCR(PTD15) = (1 << 8);
    PORTD_PCR(PTD16) = (1 << 8);

    // Configure as output
    GPIOD->PDDR |= (1 << PTD0) | (1 << PTD15) | (1 << PTD16);
}

// delay ~3s
void delay_3s(void) {
    for (volatile unsigned long i = 0; i < 300000000; i++) {
    }
}

// LED control
void LED_On(uint32_t pin) {
    GPIOD->PCOR = (1 << pin);
}

void LED_Off(uint32_t pin) {
    GPIOD->PSOR = (1 << pin);
}

int main(void) {
    GPIO_Init();

    while (1) {
        LED_On(PTD15); delay_3s();  // Red ON
        LED_Off(PTD15); delay_3s(); // Red OFF

        LED_On(PTD16); delay_3s();  // Green ON
        LED_Off(PTD16); delay_3s(); // Green OFF

        LED_On(PTD0); delay_3s();   // Blue ON
        LED_Off(PTD0); delay_3s();  // Blue OFF
    }

    return 0;
}
