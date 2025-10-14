#include "S32K144.h"
#include "hal_gpio.h"
#include "Driver_GPIO_Pins.h"
#include "Driver_GPIO.h"
#include "Driver_NVIC.h"
/* ==== Base Address ==== */
static GPIO_Type * const GPIO_BASE[] = IP_GPIO_BASE_PTRS;
static PORT_Type * const PORT_BASE[] = IP_PORT_BASE_PTRS;

/* ==== Bảng callback theo global pin ==== */
static HAL_GPIO_Callback_t gpio_cb[GPIO_TOTAL_PINS] = {0};

/* ==== Macro chuyển đổi ==== */
#define GPIO_PIN_ID(port, pin)   ((port) * 32U + (pin))

/* ==== API Implementation ==== */

int32_t HAL_GPIO_Init(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Direction_t dir, HAL_GPIO_Callback_t cb)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    uint32_t id = GPIO_PIN_ID(port, pin);
    gpio_cb[id] = cb;

    /* Set MUX to GPIO */
    PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_MUX_MASK;
    PORT_BASE[port]->PCR[pin] |= PORT_PCR_MUX(1);

    /* Set direction */
    HAL_GPIO_SetDirection(port, pin, dir);
    return 0;
}

int32_t HAL_GPIO_SetDirection(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Direction_t dir)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;
    if (dir == HAL_GPIO_DIR_OUTPUT)
        GPIO_BASE[port]->PDDR |= (1UL << pin);
    else
        GPIO_BASE[port]->PDDR &= ~(1UL << pin);
    return 0;
}

int32_t HAL_GPIO_SetOutputMode(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_OutputMode_t mode)
{
   /*do nothing */
    return 0;
}

int32_t HAL_GPIO_SetPull(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Pull_t pull)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    PORT_BASE[port]->PCR[pin] &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

    switch (pull) {
        case HAL_GPIO_PULL_UP:
            PORT_BASE[port]->PCR[pin] |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
            break;
        case HAL_GPIO_PULL_DOWN:
            PORT_BASE[port]->PCR[pin] |= (PORT_PCR_PE_MASK);
            PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
    }
    return 0;
}

int32_t HAL_GPIO_SetTrigger(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Trigger_t trigger)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;

    switch (trigger) {
        case HAL_GPIO_TRIGGER_RISING_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(9);  // rising
            break;
        case HAL_GPIO_TRIGGER_FALLING_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(10); // falling
            break;
        case HAL_GPIO_TRIGGER_EITHER_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(11); // both
            break;
        default:
            break;
    }

    /* Enable NVIC tương ứng */
    switch (port) {
        case HAL_GPIO_PORT_A: NVIC_EnableIRQ(PORTA_IRQn); break;
        case HAL_GPIO_PORT_B: NVIC_EnableIRQ(PORTB_IRQn); break;
        case HAL_GPIO_PORT_C: NVIC_EnableIRQ(PORTC_IRQn); break;
        case HAL_GPIO_PORT_D: NVIC_EnableIRQ(PORTD_IRQn); break;
        case HAL_GPIO_PORT_E: NVIC_EnableIRQ(PORTE_IRQn); break;
        default: break;
    }

    return 0;
}

void HAL_GPIO_Write(HAL_GPIO_Port_t port, uint8_t pin, uint8_t value)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return;
    if (value)
        GPIO_BASE[port]->PSOR = (1UL << pin);
    else
        GPIO_BASE[port]->PCOR = (1UL << pin);
}

uint8_t HAL_GPIO_Read(HAL_GPIO_Port_t port, uint8_t pin)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return 0;
    return (uint8_t)((GPIO_BASE[port]->PDIR >> pin) & 1U);
}

void HAL_GPIO_Toggle(HAL_GPIO_Port_t port, uint8_t pin)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return;
    GPIO_BASE[port]->PTOR = (1UL << pin);
}

/* ==== Callback registration ==== */
int32_t HAL_GPIO_RegisterCallback(uint32_t global_pin, HAL_GPIO_Callback_t cb)
{
    if (global_pin >= GPIO_TOTAL_PINS) return -1;
    gpio_cb[global_pin] = cb;
    return 0;
}

/* ==== IRQ Handler ==== */
void HAL_GPIO_IRQHandler(HAL_GPIO_Port_t port)
{
    uint32_t flags = PORT_BASE[port]->ISFR;
    if (!flags) return;
    PORT_BASE[port]->ISFR = flags;

    for (uint8_t i = 0; i < 32; i++) {
        if (flags & (1UL << i)) {
            uint32_t id = GPIO_PIN_ID(port, i);
            if (gpio_cb[id]) {
                /* Gọi callback người dùng theo chuẩn CMSIS */
                gpio_cb[id](id, ARM_GPIO_EVENT_FALLING_EDGE);
            }
        }
    }
}


