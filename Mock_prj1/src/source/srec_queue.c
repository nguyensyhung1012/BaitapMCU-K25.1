
#include "srec_queue.h"
#include <stdint.h>

static char srec_buf[SREC_MAX_LINES][SREC_LINE_MAX_LEN];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static volatile uint8_t count = 0;

void SREC_QueueInit(void)
{
    head = tail = count = 0;
}

bool SREC_QueueIsFull(void)
{
    return (count >= SREC_MAX_LINES);
}

bool SREC_QueueIsEmpty(void)
{
    return (count == 0);
}

bool SREC_QueuePush(const char *line)
{
    bool res = false;

    if (count < SREC_MAX_LINES)
    {
        uint16_t i = 0;
        while (line[i] != '\0' && i < (SREC_LINE_MAX_LEN - 1))
        {
            srec_buf[head][i] = line[i];
            i++;
        }
        srec_buf[head][i] = '\0';

        head = (head + 1U) % SREC_MAX_LINES;
        count++;
        res = true;
    }

    return res;
}

bool SREC_QueuePop(char *out_line)
{
    bool res = false;

    if (count > 0)
    {
        uint16_t i = 0;
        while (srec_buf[tail][i] != '\0' && i < (SREC_LINE_MAX_LEN - 1))
        {
            out_line[i] = srec_buf[tail][i];
            i++;
        }
        out_line[i] = '\0';

        tail = (tail + 1U) % SREC_MAX_LINES;
        count--;
        res = true;
    }

    return res;
}
