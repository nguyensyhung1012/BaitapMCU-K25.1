#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#include "S32K144.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Port định danh ==== */
typedef enum {
    HAL_GPIO_PORT_A = 0,
    HAL_GPIO_PORT_B,
    HAL_GPIO_PORT_C,
    HAL_GPIO_PORT_D,
    HAL_GPIO_PORT_E
} HAL_GPIO_Port_t;

/* ==== Cấu hình hướng ==== */
typedef enum {
    HAL_GPIO_DIR_INPUT = 0,
    HAL_GPIO_DIR_OUTPUT
} HAL_GPIO_Direction_t;

/* ==== Cấu hình mode output ==== */
typedef enum {
    HAL_GPIO_PUSH_PULL = 0,
    HAL_GPIO_OPEN_DRAIN
} HAL_GPIO_OutputMode_t;

/* ==== Cấu hình pull ==== */
typedef enum {
    HAL_GPIO_PULL_NONE = 0,
    HAL_GPIO_PULL_UP,
    HAL_GPIO_PULL_DOWN
} HAL_GPIO_Pull_t;

/* ==== Cấu hình trigger ==== */
typedef enum {
    HAL_GPIO_TRIGGER_NONE = 0,
    HAL_GPIO_TRIGGER_RISING_EDGE,
    HAL_GPIO_TRIGGER_FALLING_EDGE,
    HAL_GPIO_TRIGGER_EITHER_EDGE
} HAL_GPIO_Trigger_t;

/* ==== Kiểu callback ==== */
typedef void (*HAL_GPIO_Callback_t)(uint32_t global_pin, uint32_t event);

/* ==== API HAL ==== */
int32_t HAL_GPIO_Init(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Direction_t dir, HAL_GPIO_Callback_t cb);
int32_t HAL_GPIO_SetDirection(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Direction_t dir);
int32_t HAL_GPIO_SetOutputMode(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_OutputMode_t mode);
int32_t HAL_GPIO_SetPull(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Pull_t pull);
int32_t HAL_GPIO_SetTrigger(HAL_GPIO_Port_t port, uint8_t pin, HAL_GPIO_Trigger_t trigger);

void HAL_GPIO_Write(HAL_GPIO_Port_t port, uint8_t pin, uint8_t value);
uint8_t HAL_GPIO_Read(HAL_GPIO_Port_t port, uint8_t pin);
void HAL_GPIO_Toggle(HAL_GPIO_Port_t port, uint8_t pin);

/* ==== Hàm xử lý ngắt ==== */
void HAL_GPIO_IRQHandler(HAL_GPIO_Port_t port);

/* ==== Đăng ký callback bằng global pin ==== */
int32_t HAL_GPIO_RegisterCallback(uint32_t global_pin, HAL_GPIO_Callback_t cb);

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H_ */
