/*
 * uart_buffer.h
 *
 *  Created on: 18 thg 10, 2025
 *      Author: nguye
 */

#ifndef UART_BUFFER_H_
#define UART_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#define UART_QUEUE_SIZE   200U   /**< Max queue size (bytes) */

/**
 * @brief Initialize UART receive queue (clear head/tail)
 */
void UART_BufferInit(void);

/**
 * @brief Push one received byte into the queue
 * @param c Byte to push
 */
bool UART_BufferPush(uint8_t c);

/**
 * @brief Check if queue is empty
 * @return true if empty
 */
bool UART_BufferIsEmpty(void);

/**
 * @brief Pop one byte from the queue
 * @param c Pointer to variable to store popped byte
 * @return true if success, false if queue empty
 */

bool UART_BufferPop(uint8_t *c);


/**
 * @brief Get current number of stored bytes
 * @return Count of bytes
 */
uint16_t UART_BufferCount(void);



#endif /* INCLUDE_UART_QUEUE_H_ */
