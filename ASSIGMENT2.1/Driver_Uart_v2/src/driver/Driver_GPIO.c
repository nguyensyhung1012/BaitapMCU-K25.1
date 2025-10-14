/*
 * Driver_GPIO.c
 *
 * Tầng Driver GPIO chuẩn CMSIS cho S32K144
 * Dựa trên tầng HAL_GPIO (đã tích hợp callback xử lý ngắt)
 *
 * Author: ChatGPT - 2025
 */

#include "Driver_GPIO.h"
#include "hal_gpio.h"
#include "Driver_GPIO_Pins.h"
#include "S32K144.h"


/* ==== Triển khai các API CMSIS ==== */

/**
 * @brief Setup GPIO Interface.
 * @param pin: chỉ số pin toàn cục (0..159)
 * @param cb_event: callback CMSIS (nếu có)
 */
static int32_t GPIO_Setup(ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event)
{
    if (!GPIO_IS_VALID(pin)) return ARM_GPIO_ERROR_PIN;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    /* Đăng ký trực tiếp callback CMSIS vào HAL */
    return HAL_GPIO_Init(port, index, HAL_GPIO_DIR_INPUT, cb_event);
}

/**
 * @brief Cấu hình hướng GPIO.
 */
static int32_t GPIO_SetDirection(ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction)
{
    if (!GPIO_IS_VALID(pin)) return ARM_GPIO_ERROR_PIN;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    return HAL_GPIO_SetDirection(
        port, index,
        (direction == ARM_GPIO_OUTPUT) ? HAL_GPIO_DIR_OUTPUT : HAL_GPIO_DIR_INPUT);
}

/**
 * @brief Cấu hình mode output (push-pull hoặc open-drain)
 */
static int32_t GPIO_SetOutputMode(ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode)
{
    if (!GPIO_IS_VALID(pin)) return ARM_GPIO_ERROR_PIN;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    return HAL_GPIO_SetOutputMode(
        port, index,
        (mode == ARM_GPIO_OPEN_DRAIN) ? HAL_GPIO_OPEN_DRAIN : HAL_GPIO_PUSH_PULL);
}

/**
 * @brief Cấu hình pull-up/pull-down
 */
static int32_t GPIO_SetPullResistor(ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor)
{
    if (!GPIO_IS_VALID(pin)) return ARM_GPIO_ERROR_PIN;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    HAL_GPIO_Pull_t pull = HAL_GPIO_PULL_NONE;
    if (resistor == ARM_GPIO_PULL_UP)      pull = HAL_GPIO_PULL_UP;
    else if (resistor == ARM_GPIO_PULL_DOWN) pull = HAL_GPIO_PULL_DOWN;

    return HAL_GPIO_SetPull(port, index, pull);
}

/**
 * @brief Cấu hình trigger event (rising/falling/either)
 */
static int32_t GPIO_SetEventTrigger(ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger)
{
    if (!GPIO_IS_VALID(pin)) return ARM_GPIO_ERROR_PIN;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    HAL_GPIO_Trigger_t hal_trig = HAL_GPIO_TRIGGER_NONE;
    switch (trigger)
    {
        case ARM_GPIO_TRIGGER_RISING_EDGE:
            hal_trig = HAL_GPIO_TRIGGER_RISING_EDGE;
            break;
        case ARM_GPIO_TRIGGER_FALLING_EDGE:
            hal_trig = HAL_GPIO_TRIGGER_FALLING_EDGE;
            break;
        case ARM_GPIO_TRIGGER_EITHER_EDGE:
            hal_trig = HAL_GPIO_TRIGGER_EITHER_EDGE;
            break;
        default:
            break;
    }

    return HAL_GPIO_SetTrigger(port, index, hal_trig);
}

/**
 * @brief Ghi giá trị ra pin (Output)
 */
static void GPIO_SetOutput(ARM_GPIO_Pin_t pin, uint32_t val)
{
    if (!GPIO_IS_VALID(pin)) return;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    HAL_GPIO_Write(port, index, (uint8_t)val);
}

/**
 * @brief Đọc trạng thái pin (Input)
 */
static uint32_t GPIO_GetInput(ARM_GPIO_Pin_t pin)
{
    if (!GPIO_IS_VALID(pin)) return 0;

    HAL_GPIO_Port_t port = GPIO_PIN_PORT(pin);
    uint8_t index = GPIO_PIN_INDEX(pin);

    return HAL_GPIO_Read(port, index);
}

/* ==== Bảng driver CMSIS ==== */
ARM_DRIVER_GPIO Driver_GPIO0 = {
    GPIO_Setup,
    GPIO_SetDirection,
    GPIO_SetOutputMode,
    GPIO_SetPullResistor,
    GPIO_SetEventTrigger,
    GPIO_SetOutput,
    GPIO_GetInput
};
