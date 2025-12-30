/*
 * uart_buffer.c
 *
 *  Created on: 18 thg 10, 2025
 *      Author: nguye
 */


/**
 * @file    uart_buffer.c
 * @brief   FIFO implementation for UART receive buffer
 */
#include "uart_buffer.h"
#include <stdint.h>

/* ================== STATIC LOCAL VARIABLES ================== */
static uint8_t uart_rx_buf[UART_QUEUE_SIZE];
static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;
static volatile uint16_t count = 0;

/* ================== FUNCTION IMPLEMENTATION ================== */
void UART_BufferInit(void)
{
    head = tail = count = 0;
}

bool UART_BufferIsFull(void)
{
    return (count >= UART_QUEUE_SIZE);
}

bool UART_BufferIsEmpty(void)
{
    return (count == 0);
}

bool UART_BufferPush(uint8_t data)
{
    if (count >= UART_QUEUE_SIZE)
        return false;

    uart_rx_buf[head] = data;
    head = (head + 1U) % UART_QUEUE_SIZE;
    count++;
    return true;
}

bool UART_BufferPop(uint8_t *data)
{
    if (count == 0U)
        return false;

    *data = uart_rx_buf[tail];
    tail = (tail + 1U) % UART_QUEUE_SIZE;
    count--;
    return true;
}

