/**
 * @file    main.c
 * @brief   Main application: USART command interface to control LEDs using CMSIS-Driver.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This application demonstrates:
 *   - CMSIS USART driver for serial command input (no stdlib used)
 *   - LPIT timer as command timeout detector
 *   - CMSIS GPIO driver for LED control
 *
 * Workflow:
 *   1. User sends commands via UART (e.g., "RED ON", "LED STATUS").
 *   2. Each received character resets an LPIT timer.
 *   3. When no character is received for a few milliseconds, the command is processed.
 *   4. The LED state is updated accordingly and feedback is sent over UART.
 *
 * @note UART: LPUART1 @ 9600 baud, 8N1
 * @author
 *   Nguyen Sy Hung (refactored and documented by ChatGPT)
 * @copyright
 *   Copyright (c) 2025
 */

#include "S32K144.h"
#include "clock_and_mode.h"
#include "Driver_USART.h"
#include "Driver_NVIC.h"
#include "hal_usart.h"
#include "Led_control.h"

/* -------------------------------------------------------------------------- */
/*                                 Definitions                                */
/* -------------------------------------------------------------------------- */

/** @brief Maximum allowed command string length. */
#define MAX_CMD_LEN   50

/* -------------------------------------------------------------------------- */
/*                             External Driver API                            */
/* -------------------------------------------------------------------------- */

/** @brief CMSIS USART driver instance (LPUART1). */
extern ARM_DRIVER_USART Driver_USART1;

/* -------------------------------------------------------------------------- */
/*                              Global Variables                              */
/* -------------------------------------------------------------------------- */

/** @brief Command receive buffer. */
static volatile char rx_buffer[MAX_CMD_LEN];

/** @brief Current write index within @ref rx_buffer. */
static volatile uint32_t rx_index = 0;

/** @brief Flag set when a full command line has been received. */
static volatile uint8_t rx_done = 0;

/** @brief Temporary byte for UART reception. */
static uint8_t rx_data = 0;

/* -------------------------------------------------------------------------- */
/*                        LPIT0 Timeout Timer Functions                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize LPIT0 timer channel 0 for command timeout detection.
 *
 * LPIT0 is configured as a one-shot timer. Every time a UART character
 * is received, the timer is restarted. When it expires, it indicates
 * that the user has finished typing the command.
 */
void LPIT0_init(void)
{
    /* Enable clock for LPIT module */
    IP_PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(7);
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

    /* Enable module clocking */
    IP_LPIT0->MCR = LPIT_MCR_M_CEN_MASK;

    /* Enable interrupt for Timer 0 */
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;

    /* Enable LPIT interrupt in NVIC */
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
}

/**
 * @brief LPIT0 Channel 0 interrupt handler.
 *
 * Triggered when no UART data has been received within timeout.
 * The current command string is considered complete.
 */
void LPIT0_Ch0_IRQHandler(void)
{
    if (IP_LPIT0->MSR & LPIT_MSR_TIF0_MASK)
    {
        /* Clear interrupt flag */
        IP_LPIT0->MSR = LPIT_MSR_TIF0_MASK;

        /* Terminate the received string */
        rx_buffer[rx_index] = '\0';
        rx_done = 1;

        /* Stop the timer */
        IP_LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK;
    }
}

/* -------------------------------------------------------------------------- */
/*                            USART Event Callback                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief USART1 interrupt event callback (CMSIS standard).
 *
 * Each time a character is received, it is appended to the command buffer.
 * The LPIT timer is restarted to measure inter-character timeout.
 *
 * @param[in] event  USART event flags (see ARM_USART_EVENT_xxx).
 */
void USART1_Event(uint32_t event)
{
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        if (rx_index < MAX_CMD_LEN - 1)
            rx_buffer[rx_index++] = rx_data;

        /* Reset LPIT timer each time a character is received */
        IP_LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK;
        IP_LPIT0->TMR[0].TVAL = 6400;  /* ~ few milliseconds timeout */
        IP_LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_MODE(1) | LPIT_TMR_TCTRL_T_EN_MASK;

        /* Continue receiving next character */
        Driver_USART1.Receive(&rx_data, 1);
    }
}

/**
 * @brief Initialize CMSIS USART1 with callback and basic configuration.
 *
 * Sets asynchronous mode (8 data bits, no parity, 1 stop bit)
 * and baudrate = 9600.
 */
void USART1_Init(void)
{
    Driver_USART1.Initialize(USART1_Event);
    Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1,
                          HAL_USART_BAUDRATE_9600);
}

/* -------------------------------------------------------------------------- */
/*                                   MAIN                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Main entry point of the application.
 *
 * Initializes system clock, USART, LEDs, and timeout timer,
 * then enters a loop waiting for user commands.
 *
 * Supported commands (via UART @9600):
 *   - RED ON / RED OFF
 *   - GREEN ON / GREEN OFF
 *   - BLUE ON / BLUE OFF
 *   - LED STATUS
 *   - HELP
 *
 * @return int  (never returns)
 */
int main(void)
{
    /* === Initialize system clock === */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    /* === Initialize peripherals === */
    LED_Init();
    LPIT0_init();
    USART1_Init();

    /* Start first character reception */
    Driver_USART1.Receive(&rx_data, 1);

    /* === Main processing loop === */
    while (1)
    {
        /* Terminate the received string */
        if (rx_done)
        {
            rx_done = 0;

            char response[128];
            LED_ProcessCommand((char *)rx_buffer, response);

            /* Send processed response */
            Driver_USART1.Send(response, LED_StrLen(response));

            /* Clear buffer index and show next prompt */
            rx_index = 0;
            Driver_USART1.Send("\r\n> ", LED_StrLen("\r\n> "));
        }
        else
        {
            /* do nothing */
            
        }
    }
}
