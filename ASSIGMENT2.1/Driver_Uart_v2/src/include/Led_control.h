/**
 * @file    Led_control.h
 * @brief   LED control interface using CMSIS-Driver GPIO and USART.
 * @version 1.0
 * @date    14-Oct-2025
 * @note Compatible with CMSIS-Driver GPIO and PORT layers.
 * @author
 *   Nguyen Sy Hung
 */

#ifndef INCLUDE_LED_CONTROL_H_
#define INCLUDE_LED_CONTROL_H_

#include "Driver_GPIO.h"
#include "Driver_PORT_S32K144.h"
#include "Driver_GPIO_Pins.h"
#include "Driver_USART.h"

/**
 * @brief  Initialize GPIO pins used for LED control.
 *         Configures LED pins as output and turns all LEDs off by default.
 */
void LED_Init(void);

/**
 * @brief  Process LED control command received via UART.
 *         Executes the corresponding LED operation and generates a response string.
 *
 * @param[in]  cmd  Null-terminated command string received from UART.
 * @param[out] out  Pointer to output buffer where response string is written.
 */
void LED_ProcessCommand(const char *cmd, char *out);

/**
 * @brief  Calculate the length of a null-terminated string.
 *
 * @param[in]  s  Input string.
 * @return     String length (number of characters before '\0').
 */
uint32_t LED_StrLen(const char *s);

#endif /* INCLUDE_LED_CONTROL_H_ */
