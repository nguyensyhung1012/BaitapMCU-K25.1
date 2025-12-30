/**
 * @file    hal_usart.c
 * @brief   Hardware Abstraction Layer (HAL) for USART (LPUART0/1/2) on S32K144.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This module provides low-level control of the three LPUART peripherals
 * available on the NXP S32K144 MCU. It implements initialization, pin muxing,
 * baudrate configuration, transmit, receive, and interrupt-driven callbacks.
 *
 * @note The HAL layer is designed to be used by the CMSIS-Driver USART layer,
 *       or directly by applications requiring register-level UART access.
 *
 * Supported channels:
 *   - HAL_LPUART0 → LPUART0 (PTE0 = TX, PTE1 = RX)
 *   - HAL_LPUART1 → LPUART1 (PTC6 = TX, PTC7 = RX)
 *   - HAL_LPUART2 → LPUART2 (PTD15 = TX, PTD16 = RX)
 *
 * @author
 *   Nguyen Sy Hung
 * @copyright
 *   Copyright (c) 2025
 */

#include "hal_usart.h"
#include "Driver_NVIC.h"

/* -------------------------------------------------------------------------- */
/*                              Private Variables                              */
/* -------------------------------------------------------------------------- */

/** @brief Registered CMSIS callback for each USART channel. */
static ARM_USART_SignalEvent_t usart_cb[3] = {NULL};

/** @brief Receive buffer pointer for each channel (non-blocking mode). */
static volatile uint8_t *rx_ptr[3] = {NULL};

/** @brief Remaining bytes to receive for each channel. */
static volatile uint32_t rx_num[3] = {0};

/* -------------------------------------------------------------------------- */
/*                               Private Macros                                */
/* -------------------------------------------------------------------------- */

/** @brief Get LPUART base address from channel enum. */
#define GET_UART(ch)   ((ch)==HAL_LPUART0 ? IP_LPUART0 : (ch)==HAL_LPUART1 ? IP_LPUART1 : IP_LPUART2)

/** @brief Get PCC index from channel enum. */
#define GET_PCC_INDEX(ch) ((ch)==HAL_LPUART0 ? PCC_LPUART0_INDEX : (ch)==HAL_LPUART1 ? PCC_LPUART1_INDEX : PCC_LPUART2_INDEX)

/** @brief Get IRQ number from channel enum. */
#define GET_IRQn(ch)   ((ch)==HAL_LPUART0 ? LPUART0_RxTx_IRQn : (ch)==HAL_LPUART1 ? LPUART1_RxTx_IRQn : LPUART2_RxTx_IRQn)

/* -------------------------------------------------------------------------- */
/*                               HAL API Functions                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Register a CMSIS callback function for a USART channel.
 *
 * @param[in] ch  USART channel (HAL_LPUART0–2).
 * @param[in] cb  Callback function pointer (can be NULL).
 */
void HAL_USART_RegisterCallback(HAL_USART_Channel_t ch, ARM_USART_SignalEvent_t cb)
{
    if (ch <= HAL_LPUART2)
        usart_cb[ch] = cb;
}

/**
 * @brief Configure clock source and enable PCC for a USART channel.
 *
 * @param[in] ch   USART channel (HAL_LPUART0–2).
 * @param[in] pcs  Clock source index (0–7). Default: 6 (SPLL_DIV2_CLK).
 */
void HAL_USART_SetClockSource(HAL_USART_Channel_t ch, uint8_t pcs)
{
    if (ch > HAL_LPUART2) return;
    if (pcs > 7) pcs = 6; /* SPLL_DIV2 default */
    IP_PCC->PCCn[GET_PCC_INDEX(ch)] = PCC_PCCn_PCS(pcs) | PCC_PCCn_CGC_MASK;
}

/**
 * @brief Initialize TX/RX pins (pin mux configuration) for the given channel.
 *
 * @param[in] ch  USART channel (HAL_LPUART0–2).
 */
void HAL_USART_InitPins(HAL_USART_Channel_t ch)
{
    switch (ch)
    {
    case HAL_LPUART0:
        IP_PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;
        IP_PORTE->PCR[0] = PORT_PCR_MUX(3); /* PTE0 - TX */
        IP_PORTE->PCR[1] = PORT_PCR_MUX(3); /* PTE1 - RX */
        break;

    case HAL_LPUART1:
        IP_PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
        IP_PORTC->PCR[6] = PORT_PCR_MUX(2); /* PTC6 - TX */
        IP_PORTC->PCR[7] = PORT_PCR_MUX(2); /* PTC7 - RX */
        break;

    case HAL_LPUART2:
        IP_PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
        IP_PORTD->PCR[15] = PORT_PCR_MUX(3); /* PTD15 - TX */
        IP_PORTD->PCR[16] = PORT_PCR_MUX(3); /* PTD16 - RX */
        break;

    default:
        break;
    }
}

/**
 * @brief Enable interrupt for a specific USART channel.
 *
 * @param[in] ch  USART channel.
 */
void HAL_USART_EnableIRQ(HAL_USART_Channel_t ch)
{
    NVIC_ClearPendingIRQ(GET_IRQn(ch));
    NVIC_EnableIRQ(GET_IRQn(ch));
}

/**
 * @brief Disable interrupt for a specific USART channel.
 *
 * @param[in] ch  USART channel.
 */
void HAL_USART_DisableIRQ(HAL_USART_Channel_t ch)
{
    NVIC_DisableIRQ(GET_IRQn(ch));
}

/**
 * @brief Configure baud rate and control registers for USART channel.
 *
 * Enables TX, RX, and RX interrupt by default.
 *
 * @param[in] ch       USART channel.
 * @param[in] control  Control configuration bits (reserved).
 * @param[in] baud     Desired baud rate (enum HAL_USART_Baudrate_t).
 */
void HAL_USART_Config(HAL_USART_Channel_t ch, uint32_t control, HAL_USART_Baudrate_t baud)
{
    if (ch > HAL_LPUART2) return;

    LPUART_Type *uart = GET_UART(ch);
    uart->CTRL = 0; /* Disable before configuration */

    uint32_t periph_clk = 40000000U; /* SPLL_DIV2_CLK */
    uint32_t osr = 15;               /* 16x oversampling */
    uint32_t sbr = (periph_clk + ((osr + 1) * baud / 2)) / ((osr + 1) * baud);

    uart->BAUD = LPUART_BAUD_OSR(osr) | LPUART_BAUD_SBR(sbr);
    uart->CTRL = LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK | LPUART_CTRL_RIE_MASK;

    /* Clear status flags */
    uart->STAT |= (LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                   LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);

    NVIC_ClearPendingIRQ(GET_IRQn(ch));
    NVIC_EnableIRQ(GET_IRQn(ch));

    (void)control; /* reserved parameter for compatibility */
}

/**
 * @brief Send data (blocking mode).
 *
 * @param[in] ch    USART channel.
 * @param[in] data  Pointer to transmit buffer.
 * @param[in] num   Number of bytes to send.
 */
void HAL_USART_Send(HAL_USART_Channel_t ch, const void *data, uint32_t num)
{
    if (ch > HAL_LPUART2 || data == NULL || num == 0) return;
    LPUART_Type *uart = GET_UART(ch);
    const uint8_t *ptr = (const uint8_t *)data;

    while (num--) {
        while (!(uart->STAT & LPUART_STAT_TDRE_MASK));
        uart->DATA = *ptr++;
    }

    if (usart_cb[ch])
        usart_cb[ch](ARM_USART_EVENT_SEND_COMPLETE);
}

/**
 * @brief Start a non-blocking receive operation.
 *
 * Data will be received via interrupt and written into the provided buffer.
 *
 * @param[in] ch    USART channel.
 * @param[out] data Pointer to destination buffer.
 * @param[in] num   Number of bytes to receive.
 */
void HAL_USART_Receive(HAL_USART_Channel_t ch, void *data, uint32_t num)
{
    if (ch > HAL_LPUART2 || data == NULL || num == 0) return;
    rx_ptr[ch] = (uint8_t *)data;
    rx_num[ch] = num;
}

/**
 * @brief Internal ISR handler for a USART channel.
 *
 * Called by corresponding LPUARTx_RxTx_IRQHandler(). Handles received data
 * and invokes registered CMSIS callbacks.
 *
 * @param[in] ch  USART channel.
 */
void HAL_USART_IRQHandler(HAL_USART_Channel_t ch)
{
    if (ch > HAL_LPUART2) return;
    LPUART_Type *uart = GET_UART(ch);
    uint32_t stat = uart->STAT;

    /* Handle RX */
    if ((stat & LPUART_STAT_RDRF_MASK) && rx_ptr[ch] && rx_num[ch])
    {
        *rx_ptr[ch] = (uint8_t)uart->DATA;
        rx_ptr[ch]++;
        rx_num[ch]--;

        if (usart_cb[ch])
            usart_cb[ch](ARM_USART_EVENT_RECEIVE_COMPLETE);
    }

    /* Clear error flags */
    uart->STAT |= (LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                   LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
}

/* -------------------------------------------------------------------------- */
/*                               IRQ Definitions                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief LPUART0 interrupt handler.
 */
void LPUART0_RxTx_IRQHandler(void) { HAL_USART_IRQHandler(HAL_LPUART0); }

/**
 * @brief LPUART1 interrupt handler.
 */
void LPUART1_RxTx_IRQHandler(void) { HAL_USART_IRQHandler(HAL_LPUART1); }

/**
 * @brief LPUART2 interrupt handler.
 */
void LPUART2_RxTx_IRQHandler(void) { HAL_USART_IRQHandler(HAL_LPUART2); }
