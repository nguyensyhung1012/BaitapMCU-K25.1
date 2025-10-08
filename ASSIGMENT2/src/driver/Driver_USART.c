/**
 * @file Driver_USART.c
 * @author nguyen sy hung  (nguyensyhung10122001@gmail.com)
 * @brief CMSIS USART driver implementation for NXP S32K144 MCU.
 * @version 0.1
 * @date 2025-10-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "S32K144.h"
#include "Driver_USART.h"
#include "clock_and_mode.h"
#include "Driver_Common.h"
#include "Driver_NVIC.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPUART1_BAUDRATE 9600

/*******************************************************************************
 * Variables
 ******************************************************************************/
static ARM_USART_SignalEvent_t USART1_cb_event = NULL;
static ARM_USART_STATUS usart_status = {0};
static uint32_t tx_count = 0;
static uint32_t rx_count = 0;
uint8_t rx_data = 0; // dữ liệu nhận được mới nhất

/*******************************************************************************
 * API
 ******************************************************************************/
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_DRIVER_VERSION_MAJOR_MINOR(2, 4)
};

/* ==== Driver capabilities ==== */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    .asynchronous        = 1,
    .event_tx_complete   = 1,
    .event_rx_timeout    = 1
};
/* ==== Enable Clock and PCS ==== */
void USART1_SetClockSource(uint8_t pcs)
{
    if (pcs > 7)
    {
        pcs = 1; // fallback: SOSCDIV2_CLK
    }
    /* Bật clock cho peripheral + chọn nguồn PCS */
    IP_PCC->PCCn[PCC_LPUART1_INDEX] =
        PCC_PCCn_PCS(pcs) | PCC_PCCn_CGC_MASK;
}
/* ==== ISR ==== */
void LPUART1_RxTx_IRQHandler(void)
{
    /* --- Nhận dữ liệu --- */
    if (IP_LPUART1->STAT & LPUART_STAT_RDRF_MASK)
    {
    /* đọc dữ liệu để clear flag */
        rx_data = (uint8_t)IP_LPUART1->DATA; 
        rx_count++;
        if (USART1_cb_event)
            USART1_cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
    }

    /* --- Gửi hoàn tất --- */
    if (IP_LPUART1->STAT & LPUART_STAT_TDRE_MASK)
    {
        if (USART1_cb_event)
            USART1_cb_event(ARM_USART_EVENT_SEND_COMPLETE);
    }
}

/* ==== CMSIS API ==== */
static ARM_DRIVER_VERSION GetVersion(void) { return DriverVersion; }
static ARM_USART_CAPABILITIES GetCapabilities(void) { return DriverCapabilities; }
/* ==== Initialize the LPUART1 peripheral and register callback function ==== */
static int32_t Initialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_cb_event = cb_event;

    // PORTC6 (TX), PORTC7 (RX)
    IP_PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
    IP_PORTC->PCR[6] = PORT_PCR_MUX(2);
    IP_PORTC->PCR[7] = PORT_PCR_MUX(2);

    return ARM_DRIVER_OK;
}
/* ==== Uninitialize the LPUART1 peripheral and register callback function ==== */
static int32_t Uninitialize(void)
{
    USART1_cb_event = NULL;
    return ARM_DRIVER_OK;
}

/* ==== Enable clock for USART1 ==== */
static int32_t PowerControl(ARM_POWER_STATE state)
{
    switch (state)
    {
    case ARM_POWER_OFF:
        IP_PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
        break;

    case ARM_POWER_FULL:
        /* Clock gate control and  SOSCDIV2_CLK */
        IP_PCC->PCCn[PCC_LPUART1_INDEX] = PCC_PCCn_PCS(1) | PCC_PCCn_CGC_MASK; 
        break;

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

/*==== Send Data ====*/
static int32_t Send(const void *data, uint32_t num)
{
    const uint8_t *ptr = data;
    tx_count = 0;
    usart_status.tx_busy = 1;

    while (num--)
    {
        while (!(IP_LPUART1->STAT & LPUART_STAT_TDRE_MASK));
        IP_LPUART1->DATA = *ptr++;
        tx_count++;
    }

    usart_status.tx_busy = 0;
    if (USART1_cb_event)
        USART1_cb_event(ARM_USART_EVENT_SEND_COMPLETE);

    return ARM_DRIVER_OK;
}
/*==== Receive data ====*/
static int32_t Receive(void *data, uint32_t num)
{
    uint8_t *ptr = data;
    rx_count = 0;
    usart_status.rx_busy = 1;

    while (num--)
    {
        while (!(IP_LPUART1->STAT & LPUART_STAT_RDRF_MASK));
        *ptr++ = (uint8_t)IP_LPUART1->DATA;
        rx_count++;
    }

    usart_status.rx_busy = 0;
    if (USART1_cb_event)
        USART1_cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);

    return ARM_DRIVER_OK;
}

/* Count tx data */
static uint32_t GetTxCount(void) { return tx_count; }
/* Count rx data */
static uint32_t GetRxCount(void) { return rx_count; }

/* ======================== Control() ======================== */
static int32_t Control(uint32_t control, uint32_t arg)
{
    uint32_t mode = control & ARM_USART_CONTROL_Msk;

    switch (mode)
    {
    case ARM_USART_MODE_ASYNCHRONOUS:
    {
        uint32_t baud = arg;
        uint32_t data_bits_field = control & ARM_USART_DATA_BITS_Msk;
        uint32_t parity_field    = control & ARM_USART_PARITY_Msk;
        uint32_t stop_bits_field = control & ARM_USART_STOP_BITS_Msk;

        /* 1. Lấy clock nguồn từ PCC_LPUART1 */
        uint32_t pcs = (IP_PCC->PCCn[PCC_LPUART1_INDEX] >> 24) & 0x7;
        uint32_t periph_clk;

        switch (pcs)
        {
        case 1: periph_clk = 8000000U;  break; // SOSCDIV2_CLK = 8MHz
        case 2: periph_clk = 8000000U;  break; // SIRC_DIV2 = 8MHz
        case 3: periph_clk = 48000000U; break; // FIRC_DIV2 = 48MHz
        case 6: periph_clk = 40000000U; break; // SPLL_DIV2 = 40MHz
        case 7: periph_clk = 128000U;   break; // LPO_CLK = 128kHz
        default: periph_clk = 8000000U; break;
        }

        /* 2. Tính OSR & SBR */
        uint32_t osr = 15; // oversample 16x
        uint32_t sbr = (periph_clk + ((osr + 1) * baud / 2)) / ((osr + 1) * baud);
        if (sbr == 0 || sbr > 0x1FFF)
            return ARM_USART_ERROR_BAUDRATE;

        /* 3. Ghi thanh ghi BAUD */
        IP_LPUART1->BAUD = LPUART_BAUD_OSR(osr) | LPUART_BAUD_SBR(sbr);

        /* 4. Stop bits */
        if (stop_bits_field == ARM_USART_STOP_BITS_2)
            IP_LPUART1->BAUD |= LPUART_BAUD_SBNS_MASK;
        else
            IP_LPUART1->BAUD &= ~LPUART_BAUD_SBNS_MASK;

        /* 5. Parity */
        IP_LPUART1->CTRL &= ~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK);
        if (parity_field == ARM_USART_PARITY_EVEN)
            IP_LPUART1->CTRL |= LPUART_CTRL_PE_MASK;
        else if (parity_field == ARM_USART_PARITY_ODD)
            IP_LPUART1->CTRL |= (LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK);

        /* 6. Data bits */
        if (data_bits_field == ARM_USART_DATA_BITS_9)
            IP_LPUART1->CTRL |= LPUART_CTRL_M_MASK;
        else
            IP_LPUART1->CTRL &= ~LPUART_CTRL_M_MASK;

        /* 7. Enable TX/RX/Interrupt */
        IP_LPUART1->CTRL |= LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK | LPUART_CTRL_RIE_MASK;
        NVIC_EnableIRQ(LPUART1_RxTx_IRQn);

        return ARM_DRIVER_OK;
    }

    case ARM_USART_CONTROL_TX:
        if (arg)
            IP_LPUART1->CTRL |= LPUART_CTRL_TE_MASK;
        else
            IP_LPUART1->CTRL &= ~LPUART_CTRL_TE_MASK;
        break;

    case ARM_USART_CONTROL_RX:
        if (arg)
            IP_LPUART1->CTRL |= LPUART_CTRL_RE_MASK;
        else
            IP_LPUART1->CTRL &= ~LPUART_CTRL_RE_MASK;
        break;

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/*==== Get status ====*/
static ARM_USART_STATUS GetStatus(void) { return usart_status; }


/* ==== Export driver ==== */
const ARM_DRIVER_USART Driver_USART1 = {
    GetVersion,
    GetCapabilities,
    Initialize,
    Uninitialize,
    PowerControl,
    Send,
    Receive,
    NULL,
    GetTxCount,
    GetRxCount,
    Control,
    GetStatus,
    NULL,
    NULL
};
