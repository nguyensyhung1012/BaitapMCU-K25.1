/**
 * @file    hal_usart.h
 * @brief   Hardware Abstraction Layer for USART (LPUART0–2) on S32K144.
 *
 * Provides an interface for configuring and operating USART peripherals,
 * abstracting hardware-specific details from upper layers (e.g. CMSIS driver).
 */

#ifndef HAL_USART_H_
#define HAL_USART_H_

#include "S32K144.h"
#include "Driver_USART.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 *                  TYPE DEFINITIONS
 * ============================================================ */

/**
 * @brief Supported USART channels.
 */
typedef enum {
    HAL_LPUART0 = 0,
    HAL_LPUART1,
    HAL_LPUART2
} HAL_USART_Channel_t;

/**
 * @brief Supported baud rates.
 */
typedef enum {
    HAL_USART_BAUDRATE_9600   = 9600U,
    HAL_USART_BAUDRATE_19200  = 19200U,
    HAL_USART_BAUDRATE_57600  = 57600U,
    HAL_USART_BAUDRATE_115200 = 115200U,
    HAL_USART_BAUDRATE_230400 = 230400U,
    HAL_USART_BAUDRATE_460800 = 460800U,
    HAL_USART_BAUDRATE_921600 = 921600U
} HAL_USART_Baudrate_t;

/* ============================================================
 *                  FUNCTION PROTOTYPES
 * ============================================================ */

/**
 * @brief Register a CMSIS-style callback function for a USART channel.
 */
void HAL_USART_RegisterCallback(HAL_USART_Channel_t ch, ARM_USART_SignalEvent_t cb);

/**
 * @brief Configure the clock source for the specified USART channel.
 * @param ch  Channel (LPUART0–2)
 * @param pcs Peripheral clock source selector (e.g. SPLL_DIV2 = 6)
 */
void HAL_USART_SetClockSource(HAL_USART_Channel_t ch, uint8_t pcs);

/**
 * @brief Initialize UART TX/RX pins for the specified channel.
 */
void HAL_USART_InitPins(HAL_USART_Channel_t ch);

/**
 * @brief Enable/Disable interrupts for the specified channel.
 */
void HAL_USART_EnableIRQ(HAL_USART_Channel_t ch);
void HAL_USART_DisableIRQ(HAL_USART_Channel_t ch);

/**
 * @brief Configure baudrate and control mode (asynchronous, etc.)
 */
void HAL_USART_Config(HAL_USART_Channel_t ch, uint32_t control, HAL_USART_Baudrate_t baud);

/**
 * @brief Send data (blocking or interrupt-driven, depending on HAL implementation).
 */
void HAL_USART_Send(HAL_USART_Channel_t ch, const void *data, uint32_t num);

/**
 * @brief Receive data (blocking or interrupt-driven).
 */
void HAL_USART_Receive(HAL_USART_Channel_t ch, void *data, uint32_t num);

/**
 * @brief Common interrupt handler for all USART channels.
 */
void HAL_USART_IRQHandler(HAL_USART_Channel_t ch);

/* ============================================================
 *                  INTERRUPT HANDLERS (ISR ENTRY POINTS)
 * ============================================================ */
void LPUART0_RxTx_IRQHandler(void);
void LPUART1_RxTx_IRQHandler(void);
void LPUART2_RxTx_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_USART_H_ */