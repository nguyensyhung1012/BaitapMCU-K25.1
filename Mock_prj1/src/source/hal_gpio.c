/**
 * @file    hal_gpio.c
 * @brief   Hardware Abstraction Layer implementation for GPIO on S32K144.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This file provides low-level access to the GPIO and PORT peripherals
 * of the NXP S32K144 MCU. It implements initialization, configuration,
 * and interrupt handling for general-purpose I/O pins.
 *
 * @note This module is used by the CMSIS-Driver GPIO layer and can also
 *       be used directly by applications that need direct register-level
 *       GPIO control.
 *
 * @author
 *   Nguyen Sy Hung
 * @copyright
 *   Copyright (c) 2025
 */

#include "S32K144.h"
#include "hal_gpio.h"
#include "Driver_GPIO_Pins.h"
#include "Driver_GPIO.h"
#include "Driver_NVIC.h"

/* -------------------------------------------------------------------------- */
/*                              Private Variables                              */
/* -------------------------------------------------------------------------- */

/** @brief GPIO base address table (Port A–E). */
static GPIO_Type * const GPIO_BASE[] = IP_GPIO_BASE_PTRS;
/** @brief PORT base address table (Port A–E). */
static PORT_Type * const PORT_BASE[] = IP_PORT_BASE_PTRS;

/** @brief Global callback table indexed by global pin ID (port * 32 + pin). */
static HAL_GPIO_Callback_t gpio_cb[GPIO_TOTAL_PINS] = {0};

/** @brief Helper macro for computing global pin ID. */
#define GPIO_PIN_ID(port, pin)   ((port) * 32U + (pin))

/* -------------------------------------------------------------------------- */
/*                             HAL API Implementation                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize a GPIO pin with given configuration.
 *
 * Sets the pin function to GPIO mode, configures its direction,
 * and registers an optional callback for interrupt handling.
 *
 * @param[in] port  GPIO port (A–E).
 * @param[in] pin   Pin number (0–31).
 * @param[in] dir   Direction (input or output).
 * @param[in] cb    Optional callback function for interrupts (can be NULL).
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_Init(HAL_GPIO_Port_t port, uint8_t pin,
                      HAL_GPIO_Direction_t dir, HAL_GPIO_Callback_t cb)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    uint32_t id = GPIO_PIN_ID(port, pin);
    gpio_cb[id] = cb;

    /* Configure pin MUX as GPIO */
    PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_MUX_MASK;
    PORT_BASE[port]->PCR[pin] |= PORT_PCR_MUX(1);

    /* Configure direction */
    HAL_GPIO_SetDirection(port, pin, dir);
    return 0;
}

/**
 * @brief Configure GPIO pin direction.
 *
 * @param[in] port  GPIO port (A–E).
 * @param[in] pin   Pin number (0–31).
 * @param[in] dir   Direction: input or output.
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_SetDirection(HAL_GPIO_Port_t port, uint8_t pin,
                              HAL_GPIO_Direction_t dir)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;
    if (dir == HAL_GPIO_DIR_OUTPUT)
        GPIO_BASE[port]->PDDR |= (1UL << pin);
    else
        GPIO_BASE[port]->PDDR &= ~(1UL << pin);
    return 0;
}

/**
 * @brief Configure output mode for a GPIO pin.
 *
 * Currently not implemented (push-pull is default on S32K144).
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin number.
 * @param[in] mode  Output mode (push-pull or open-drain).
 * @retval  0       Operation successful (no action taken).
 */
int32_t HAL_GPIO_SetOutputMode(HAL_GPIO_Port_t port, uint8_t pin,
                               HAL_GPIO_OutputMode_t mode)
{
    (void)port; (void)pin; (void)mode;
    /* No open-drain support implemented in this HAL. */
    return 0;
}

/**
 * @brief Configure pull-up or pull-down resistor for a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin number.
 * @param[in] pull  Pull resistor configuration.
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_SetPull(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Pull_t pull)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    PORT_BASE[port]->PCR[pin] &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

    switch (pull) {
        case HAL_GPIO_PULL_UP:
            PORT_BASE[port]->PCR[pin] |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
            break;
        case HAL_GPIO_PULL_DOWN:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
    }
    return 0;
}

/**
 * @brief Configure interrupt trigger type for a GPIO pin.
 *
 * @param[in] port     GPIO port.
 * @param[in] pin      Pin number.
 * @param[in] trigger  Trigger type (rising, falling, or both edges).
 * @retval  0          Operation successful.
 * @retval -1          Invalid parameter.
 */
int32_t HAL_GPIO_SetTrigger(HAL_GPIO_Port_t port, uint8_t pin,
                            HAL_GPIO_Trigger_t trigger)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return -1;

    PORT_BASE[port]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;

    switch (trigger) {
        case HAL_GPIO_TRIGGER_RISING_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(9);  /* Rising edge */
            break;
        case HAL_GPIO_TRIGGER_FALLING_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(10); /* Falling edge */
            break;
        case HAL_GPIO_TRIGGER_EITHER_EDGE:
            PORT_BASE[port]->PCR[pin] |= PORT_PCR_IRQC(11); /* Both edges */
            break;
        default:
            break;
    }

    /* Enable NVIC interrupt for corresponding port */
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

/**
 * @brief Write logical value to a GPIO output pin.
 *
 * @param[in] port   GPIO port.
 * @param[in] pin    Pin number.
 * @param[in] value  Logical output (0 = low, 1 = high).
 */
void HAL_GPIO_Write(HAL_GPIO_Port_t port, uint8_t pin, uint8_t value)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return;
    if (value)
        GPIO_BASE[port]->PCOR = (1UL << pin);
    else
        GPIO_BASE[port]->PSOR = (1UL << pin);
}

/**
 * @brief Read the logical value of a GPIO input pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin number.
 * @return uint8_t  Input state (0 = low, 1 = high).
 */
uint8_t HAL_GPIO_Read(HAL_GPIO_Port_t port, uint8_t pin)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return 0;
    return (uint8_t)((GPIO_BASE[port]->PDIR >> pin) & 1U);
}

/**
 * @brief Toggle the output state of a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin number.
 */
void HAL_GPIO_Toggle(HAL_GPIO_Port_t port, uint8_t pin)
{
    if (port > HAL_GPIO_PORT_E || pin >= 32) return;
    GPIO_BASE[port]->PTOR = (1UL << pin);
}

/**
 * @brief Register a GPIO callback for a global pin ID.
 *
 * @param[in] global_pin  Global pin ID (port * 32 + pin).
 * @param[in] cb          Callback function pointer.
 * @retval  0             Operation successful.
 * @retval -1             Invalid parameter.
 */
int32_t HAL_GPIO_RegisterCallback(uint32_t global_pin, HAL_GPIO_Callback_t cb)
{
    if (global_pin >= GPIO_TOTAL_PINS) return -1;
    gpio_cb[global_pin] = cb;
    return 0;
}

/**
 * @brief Common interrupt handler for GPIO ports.
 *
 * Should be called by the corresponding PORTx_IRQHandler().
 * It checks ISFR flags, clears them, and invokes registered callbacks.
 *
 * @param[in] port  GPIO port where interrupt occurred.
 */
void HAL_GPIO_IRQHandler(HAL_GPIO_Port_t port)
{
    uint32_t flags = PORT_BASE[port]->ISFR;
    if (!flags) return;

    /* Clear interrupt flags */
    PORT_BASE[port]->ISFR = flags;

    for (uint8_t i = 0; i < 32; i++) {
        if (flags & (1UL << i)) {
            uint32_t id = GPIO_PIN_ID(port, i);
            if (gpio_cb[id]) {
                /* Invoke user callback (CMSIS event convention) */
                gpio_cb[id](id, ARM_GPIO_EVENT_FALLING_EDGE);
            }
        }
    }
}
