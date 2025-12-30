/**
 * @file    Driver_USART.c
 * @brief   CMSIS-Driver implementation for USART peripheral on S32K144.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This file implements the CMSIS USART driver API and delegates
 * all hardware access and interrupt handling to the HAL layer.
 *
 * Supported channel in this implementation: HAL_LPUART1
 *
 * @note This layer provides CMSIS compatibility only (no direct register access).
 *       Actual hardware operations are implemented in the HAL USART module.
 *
 * @author
 *   Nguyen Sy Hung
 * @copyright
 *   Copyright (c) 2025
 */

#include "Driver_USART.h"
#include "hal_usart.h"
#include "S32K144.h"

/* -------------------------------------------------------------------------- */
/*                              Driver Constants                               */
/* -------------------------------------------------------------------------- */

/** @brief CMSIS Driver version (major.minor). */
#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/** @brief Driver version structure. */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/** @brief Driver capability structure (asynchronous UART only). */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    .asynchronous          = 1,  /* Supports UART mode only */
    .synchronous_master    = 0,
    .synchronous_slave     = 0,
    .single_wire           = 0,
    .irda                  = 0,
    .smart_card            = 0,
    .smart_card_clock      = 0,
    .flow_control_rts      = 0,
    .flow_control_cts      = 0,
    .event_tx_complete     = 1,
    .event_rx_timeout      = 1,
    /* Remaining fields default to 0 */
};

/* -------------------------------------------------------------------------- */
/*                       CMSIS USART Driver API Implementation                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get version information of the CMSIS USART driver.
 * @return ARM_DRIVER_VERSION structure containing API and driver version.
 */
static ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @brief Get driver capabilities.
 * @return ARM_USART_CAPABILITIES structure.
 */
static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @brief Initialize USART peripheral and register callback function.
 *
 * This function configures the LPUART1 channel clock source and pinmux,
 * and registers a user callback for communication events.
 *
 * @param[in] cb_event  Pointer to CMSIS callback function.
 * @retval ARM_DRIVER_OK  Operation successful.
 */
static int32_t ARM_USART_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    /* HAL layer manages callback and interrupt setup */
    HAL_USART_RegisterCallback(HAL_LPUART1, cb_event);

    /* Initialize clock source and pins */
    HAL_USART_SetClockSource(HAL_LPUART1, 6);   /* SPLL_DIV2 = 40 MHz */
    HAL_USART_InitPins(HAL_LPUART1);

    return ARM_DRIVER_OK;
}

/**
 * @brief Uninitialize USART peripheral.
 *
 * Disables interrupts and unregisters any previously set callback.
 *
 * @retval ARM_DRIVER_OK  Operation successful.
 */
static int32_t ARM_USART_Uninitialize(void)
{
    HAL_USART_DisableIRQ(HAL_LPUART1);
    HAL_USART_RegisterCallback(HAL_LPUART1, NULL);
    return ARM_DRIVER_OK;
}

/**
 * @brief Control power state of USART peripheral.
 *
 * (Power management not implemented for S32K144; always returns OK.)
 *
 * @param[in] state  Power state.
 * @retval ARM_DRIVER_OK  Operation successful.
 */
static int32_t ARM_USART_PowerControl(ARM_POWER_STATE state)
{
    /*do nothing*/
    return ARM_DRIVER_OK;
}

/* -------------------------------------------------------------------------- */
/*                               Data Operations                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send data over USART (blocking mode).
 *
 * @param[in] data  Pointer to data buffer.
 * @param[in] num   Number of bytes to send.
 * @retval ARM_DRIVER_OK  Operation successful.
 */
static int32_t ARM_USART_Send(const void *data, uint32_t num)
{
    HAL_USART_Send(HAL_LPUART1, data, num);
    return ARM_DRIVER_OK;
}

/**
 * @brief Receive data over USART (non-blocking, interrupt-driven).
 *
 * The receive operation completes when the specified number of bytes
 * is received, or the callback event is triggered.
 *
 * @param[out] data  Pointer to destination buffer.
 * @param[in]  num   Number of bytes to receive.
 * @retval ARM_DRIVER_OK  Operation successful.
 */
static int32_t ARM_USART_Receive(void *data, uint32_t num)
{
    HAL_USART_Receive(HAL_LPUART1, data, num);
    return ARM_DRIVER_OK;
}

/**
 * @brief Full-duplex transfer (not supported for UART mode).
 *
 * @param[in]  data_out  Pointer to transmit buffer.
 * @param[out] data_in   Pointer to receive buffer.
 * @param[in]  num       Number of bytes to transfer.
 * @retval ARM_DRIVER_ERROR_UNSUPPORTED  Operation not supported.
 */
static int32_t ARM_USART_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    (void)data_out; (void)data_in; (void)num;
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @brief Get number of transmitted bytes.
 * @retval 0  (Not implemented, returns 0)
 */
static uint32_t ARM_USART_GetTxCount(void) { return 0; }

/**
 * @brief Get number of received bytes.
 * @retval 0  (Not implemented, returns 0)
 */
static uint32_t ARM_USART_GetRxCount(void) { return 0; }

/* -------------------------------------------------------------------------- */
/*                      Configuration and Status Functions                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Control USART configuration and operation modes.
 *
 * This function configures the LPUART peripheral in asynchronous mode,
 * sets the baud rate, and enables interrupts.
 *
 * @param[in] control  CMSIS control flags.
 * @param[in] arg      Argument for selected control operation (e.g., baudrate).
 * @retval ARM_DRIVER_OK                 Operation successful.
 * @retval ARM_DRIVER_ERROR_UNSUPPORTED  Unsupported mode.
 */
static int32_t ARM_USART_Control(uint32_t control, uint32_t arg)
{
    uint32_t mode = control & ARM_USART_CONTROL_Msk;

    if (mode == ARM_USART_MODE_ASYNCHRONOUS)
    {
        HAL_USART_Config(HAL_LPUART1, control, arg);
        HAL_USART_EnableIRQ(HAL_LPUART1);
        return ARM_DRIVER_OK;
    }

    if (mode == ARM_USART_CONTROL_TX || mode == ARM_USART_CONTROL_RX)
        return ARM_DRIVER_OK;

    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @brief Get current USART status.
 * @return ARM_USART_STATUS structure with all fields cleared (default).
 */
static ARM_USART_STATUS ARM_USART_GetStatus(void)
{
    ARM_USART_STATUS st = {0};
    return st;
}

/* -------------------------------------------------------------------------- */
/*                           Modem Control Functions                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Set modem control signals (not supported on S32K144).
 * @param[in] control  Modem control signal.
 * @retval ARM_DRIVER_ERROR_UNSUPPORTED
 */
static int32_t ARM_USART_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    (void)control;
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @brief Get modem status (not supported on S32K144).
 * @return ARM_USART_MODEM_STATUS structure with all fields cleared.
 */
static ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS st = {0};
    return st;
}

/* -------------------------------------------------------------------------- */
/*                           Exported Driver Instance                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief CMSIS USART Driver instance (LPUART1).
 *
 * Provides all function pointers required by CMSIS USART API.
 */
ARM_DRIVER_USART Driver_USART1 = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART_Initialize,
    ARM_USART_Uninitialize,
    ARM_USART_PowerControl,
    ARM_USART_Send,
    ARM_USART_Receive,
    ARM_USART_Transfer,
    ARM_USART_GetTxCount,
    ARM_USART_GetRxCount,
    ARM_USART_Control,
    ARM_USART_GetStatus,
    ARM_USART_SetModemControl,
    ARM_USART_GetModemStatus
};
