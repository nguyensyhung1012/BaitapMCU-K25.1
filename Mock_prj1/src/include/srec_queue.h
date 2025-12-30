/**
 * @file    srec_queue.h
 * @brief   Queue lưu tối đa 4 dòng SREC (chuỗi ASCII)
 */

#ifndef SREC_QUEUE_H_
#define SREC_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

#define SREC_MAX_LINES      4
#define SREC_LINE_MAX_LEN   256

void SREC_QueueInit(void);
bool SREC_QueuePush(const char *line);
bool SREC_QueuePop(char *out_line);
bool SREC_QueueIsEmpty(void);
bool SREC_QueueIsFull(void);
uint8_t SREC_QueueCount(void);

#endif /* SREC_QUEUE_H_ */
