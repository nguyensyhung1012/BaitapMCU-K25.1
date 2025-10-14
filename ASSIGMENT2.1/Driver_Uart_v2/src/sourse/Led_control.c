/**
 * @file    Led_control.c
 * @brief   LED control module using CMSIS-Driver GPIO and USART.
 * @version 1.0
 * @date    14-Oct-2025
 *
 * This module provides high-level LED control functionality
 * using GPIO pins defined in Driver_GPIO_Pins.h.
 * Commands are received via USART and processed to control
 * RED, GREEN, and BLUE LEDs.
 *
 * @note Uses CMSIS-Driver GPIO (Driver_GPIO0) and USART (Driver_USART1).
 *
 * @author
 *   Nguyen Sy Hung (refactored and documented by ChatGPT)
 * @copyright
 *   Copyright (c) 2025
 */

#include "Led_control.h"
#include "Driver_GPIO_Pins.h"
#include "Driver_GPIO.h"
#include "Driver_PORT_S32K144.h"
#include "Driver_USART.h"

/* -------------------------------------------------------------------------- */
/*                          External Driver References                        */
/* -------------------------------------------------------------------------- */

/** @brief CMSIS GPIO driver instance. */
extern ARM_DRIVER_GPIO  Driver_GPIO0;

/** @brief CMSIS PORT driver instance (for mux control). */
extern ARM_DRIVER_PORT  Driver_PORT0;

/** @brief CMSIS USART driver instance (for command I/O). */
extern ARM_DRIVER_USART Driver_USART1;

/* -------------------------------------------------------------------------- */
/*                             Internal Utilities                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Compare two strings for equality.
 * @param[in] a  First string.
 * @param[in] b  Second string.
 * @retval 1  If both strings are identical.
 * @retval 0  Otherwise.
 */
static int str_equal(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    return (*a == *b);
}

/**
 * @brief Copy a string from source to destination.
 * @param[out] dst  Destination buffer.
 * @param[in]  src  Source string.
 */
static void str_copy(char *dst, const char *src)
{
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
}

/**
 * @brief Calculate the length of a string (excluding null terminator).
 * @param[in] s  Input string.
 * @return String length in bytes.
 */
uint32_t LED_StrLen(const char *s)
{
    uint32_t len = 0;
    while (*s++)
        len++;
    return len;
}

/* -------------------------------------------------------------------------- */
/*                             LED Initialization                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize GPIO pins for LED control.
 *
 * Configures PORTD pins as GPIO outputs and ensures all LEDs
 * are turned OFF initially (active high logic).
 */
void LED_Init(void)
{
    /* Enable clock for PORTD */
    Driver_PORT0.EnableClock(PORTD_INDEX);

    /* Configure MUX for LED pins as GPIO */
    Driver_PORT0.SetMux(GPIO_PIN_LED_RED,   ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_GREEN, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_BLUE,  ARM_PORT_MUX_GPIO);

    /* Initialize GPIO pins (register callbacks = NULL) */
    Driver_GPIO0.Setup(GPIO_PIN_LED_RED,   NULL);
    Driver_GPIO0.Setup(GPIO_PIN_LED_GREEN, NULL);
    Driver_GPIO0.Setup(GPIO_PIN_LED_BLUE,  NULL);

    /* Configure as output */
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_RED,   ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_GREEN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_BLUE,  ARM_GPIO_OUTPUT);

    /* Turn OFF all LEDs initially (active high → write 1) */
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED,   1);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 1);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE,  1);
}

/* -------------------------------------------------------------------------- */
/*                              Command Handler                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Process a text command to control LEDs.
 *
 * The function compares the input command string with predefined
 * commands such as `"RED ON"`, `"LED STATUS"`, etc., and performs
 * the corresponding LED control action.
 *
 * @param[in]  cmd  Input command string (null-terminated).
 * @param[out] out  Response string to be sent over USART.
 *
 * @note Commands supported:
 *   - RED ON / RED OFF
 *   - GREEN ON / GREEN OFF
 *   - BLUE ON / BLUE OFF
 *   - LED STATUS
 *   - HELP
 */
void LED_ProcessCommand(const char *cmd, char *out)
{
    /* led status */
    uint32_t red_status  ; 
    uint32_t green_status ;
    uint32_t blue_status ; 
    /* === LED ON/OFF commands === */
    if (str_equal(cmd, "RED ON"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, 0);
        str_copy(out, "");
    }
    else if (str_equal(cmd, "RED OFF"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, 1);
        str_copy(out, "");
    }
    else if (str_equal(cmd, "GREEN ON"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 0);
        str_copy(out, "");
    }
    else if (str_equal(cmd, "GREEN OFF"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 1);
        str_copy(out, "");
    }
    else if (str_equal(cmd, "BLUE ON"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 0);
        str_copy(out, "");
    }
    else if (str_equal(cmd, "BLUE OFF"))
    {
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 1);
        str_copy(out, "");
    }

    /* === LED STATUS command === */
    else if (str_equal(cmd, "LED STATUS"))
    {
        /* Prompt symbol */
        Driver_USART1.Send("\r\n> ", LED_StrLen("\r\n> "));

        /* Read actual logic state (active high → 1 = off) */
         red_status   = Driver_GPIO0.GetInput(GPIO_PIN_LED_RED);
         green_status = Driver_GPIO0.GetInput(GPIO_PIN_LED_GREEN);
         blue_status  = Driver_GPIO0.GetInput(GPIO_PIN_LED_BLUE);

        char *p = out;
        str_copy(p, "LED STATUS: ");
        p += LED_StrLen(p);

        *p++ = 'R'; *p++ = '='; *p++ = red_status ? '0' : '1'; *p++ = ',';
        *p++ = ' ';
        *p++ = 'G'; *p++ = '='; *p++ = green_status ? '0' : '1'; *p++ = ',';
        *p++ = ' ';
        *p++ = 'B'; *p++ = '='; *p++ = blue_status ? '0' : '1';
        *p++ = '\r'; *p++ = '\n';
        *p = '\0';
    }

    /* === HELP command === */
    else if (str_equal(cmd, "HELP"))
    {
        Driver_USART1.Send("\r\n> ", LED_StrLen("\r\n> "));
        str_copy(out,
            "Commands:\r\n"
            " RED ON / RED OFF\r\n"
            " GREEN ON / GREEN OFF\r\n"
            " BLUE ON / BLUE OFF\r\n"
            " LED STATUS\r\n"
            " HELP\r\n");
    }

    /* === Unknown command === */
    else
    {
        Driver_USART1.Send("\r\n> ", LED_StrLen("\r\n> "));
        str_copy(out, "Command not available\r\n");
    }
}
