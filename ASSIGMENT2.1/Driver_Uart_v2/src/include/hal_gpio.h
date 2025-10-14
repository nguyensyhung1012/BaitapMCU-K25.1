/**
 * @file    hal_gpio.h
 * @brief   Hardware Abstraction Layer (HAL) for General Purpose I/O (GPIO) on S32K144.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This file defines the HAL interface for GPIO control on the NXP S32K144 MCU.
 * It provides low-level APIs to configure pin direction, pull resistors,
 * output mode, and interrupt triggers. It also manages GPIO callbacks and
 * interrupt handling used by the CMSIS-Driver GPIO layer.
 *
 * @note This HAL layer directly accesses PORT and GPIO peripheral registers.
 *       It is intended to be used by CMSIS-Driver GPIO implementations
 *       or application layers requiring fine-grained control.
 *
 * @author
 *   Nguyen Sy Hung
 * @copyright
 *   Copyright (c) 2025
 */

#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#include "S32K144.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO port identifiers.
 */
typedef enum {
    /* Port A */
    HAL_GPIO_PORT_A = 0,
    /* Port B */  
    HAL_GPIO_PORT_B,
    /* Port C */      
    HAL_GPIO_PORT_C,
    /* Port D */      
    HAL_GPIO_PORT_D,
     /* Port E */      
    HAL_GPIO_PORT_E       
} HAL_GPIO_Port_t;

/**
 * @brief GPIO pin direction configuration.
 */
typedef enum {
    HAL_GPIO_DIR_INPUT = 0,  
    HAL_GPIO_DIR_OUTPUT      
} HAL_GPIO_Direction_t;

/**
 * @brief GPIO output mode configuration.
 */
typedef enum {
    HAL_GPIO_PUSH_PULL = 0,  
    HAL_GPIO_OPEN_DRAIN      
} HAL_GPIO_OutputMode_t;

/**
 * @brief GPIO internal pull resistor configuration.
 */
typedef enum {
    HAL_GPIO_PULL_NONE = 0,  
    HAL_GPIO_PULL_UP,        
    HAL_GPIO_PULL_DOWN       
} HAL_GPIO_Pull_t;

/**
 * @brief GPIO interrupt trigger type.
 */
typedef enum {
    HAL_GPIO_TRIGGER_NONE = 0,         
    HAL_GPIO_TRIGGER_RISING_EDGE,     
    HAL_GPIO_TRIGGER_FALLING_EDGE,     
    HAL_GPIO_TRIGGER_EITHER_EDGE       
} HAL_GPIO_Trigger_t;

/**
 * @brief GPIO event callback function type.
 * @param[in] global_pin  Global pin ID (port * 32 + pin).
 * @param[in] event       Event flags or trigger source.
 */
typedef void (*HAL_GPIO_Callback_t)(uint32_t global_pin, uint32_t event);

/* -------------------------------------------------------------------------- */
/*                               HAL API FUNCTIONS                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize a GPIO pin with specified configuration.
 *
 * @param[in] port  GPIO port (A–E).
 * @param[in] pin   Pin number within the port (0–31).
 * @param[in] dir   Direction (input or output).
 * @param[in] cb    Optional callback function pointer (for interrupts).
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_Init(HAL_GPIO_Port_t port, uint8_t pin,
                      HAL_GPIO_Direction_t dir, HAL_GPIO_Callback_t cb);

/**
 * @brief Configure the direction of a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin index.
 * @param[in] dir   Input or output mode.
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_SetDirection(HAL_GPIO_Port_t port, uint8_t pin,
                              HAL_GPIO_Direction_t dir);

/**
 * @brief Configure the output mode of a GPIO pin (push-pull or open-drain).
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin index.
 * @param[in] mode  Output mode.
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_SetOutputMode(HAL_GPIO_Port_t port, uint8_t pin,
                               HAL_GPIO_OutputMode_t mode);

/**
 * @brief Configure internal pull resistor for a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin index.
 * @param[in] pull  Pull configuration (none, up, down).
 * @retval  0       Operation successful.
 * @retval -1       Invalid parameter.
 */
int32_t HAL_GPIO_SetPull(HAL_GPIO_Port_t port, uint8_t pin,
                         HAL_GPIO_Pull_t pull);

/**
 * @brief Configure interrupt trigger type for a GPIO pin.
 *
 * @param[in] port     GPIO port.
 * @param[in] pin      Pin index.
 * @param[in] trigger  Trigger mode.
 * @retval  0          Operation successful.
 * @retval -1          Invalid parameter.
 */
int32_t HAL_GPIO_SetTrigger(HAL_GPIO_Port_t port, uint8_t pin,
                            HAL_GPIO_Trigger_t trigger);

/**
 * @brief Write a logical value to a GPIO output pin.
 *
 * @param[in] port   GPIO port.
 * @param[in] pin    Pin index.
 * @param[in] value  Output value (0 = low, 1 = high).
 */
void HAL_GPIO_Write(HAL_GPIO_Port_t port, uint8_t pin, uint8_t value);

/**
 * @brief Read the logical input value of a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin index.
 * @return uint8_t  Pin value (0 = low, 1 = high).
 */
uint8_t HAL_GPIO_Read(HAL_GPIO_Port_t port, uint8_t pin);

/**
 * @brief Toggle the output state of a GPIO pin.
 *
 * @param[in] port  GPIO port.
 * @param[in] pin   Pin index.
 */
void HAL_GPIO_Toggle(HAL_GPIO_Port_t port, uint8_t pin);

/**
 * @brief Handle GPIO interrupt for a specific port.
 *
 * This function should be called from the corresponding
 * PORTx_IRQHandler() in the CMSIS driver.
 *
 * @param[in] port  GPIO port where the interrupt occurred.
 */
void HAL_GPIO_IRQHandler(HAL_GPIO_Port_t port);

/**
 * @brief Register a callback for a specific global GPIO pin.
 *
 * @param[in] global_pin  Global pin ID (port * 32 + pin).
 * @param[in] cb          Callback function pointer.
 * @retval  0             Operation successful.
 * @retval -1             Invalid parameter.
 */
int32_t HAL_GPIO_RegisterCallback(uint32_t global_pin, HAL_GPIO_Callback_t cb);

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H_ */
