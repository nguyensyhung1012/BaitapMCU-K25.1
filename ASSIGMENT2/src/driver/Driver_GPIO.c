/*
 * Driver_GPIO.c
 *
 *  Created on: 25 thg 9, 2025
 *      Author: nguyen sy hung

 * GPIO driver cho S32K144 theo chuẩn CMSIS.
 * Chỉ quản lý logic GPIO (input/output).
 * Các chức năng PORT (clock, mux, pull, interrupt config) được tách riêng
 * sang Driver_PORT để dễ sử dụng.
 */

#include "Driver_GPIO.h"
#include "S32K144.h"

/* Tổng số GPIO pins: 5 port (A..E) * 32 pin */
#define GPIO_MAX_PINS   160U

/* Tính toán số port từ chỉ số pin (0:A,1:B,2:C,3:D,4:E) */
#define PIN_PORT(pin)   ((pin) / 32U)

/* Tính toán chỉ số pin trong port (0..31) */
#define PIN_INDEX(pin)  ((pin) % 32U)

/* Bảng base address cho tất cả GPIO ports */
static GPIO_Type * const GPIO_BASE[] = IP_GPIO_BASE_PTRS;

/* Lưu callback cho từng pin (nếu sử dụng signal event) */
static ARM_GPIO_SignalEvent_t gpio_cb[GPIO_MAX_PINS];

/**
 * @brief Khởi tạo GPIO pin
 * @param[in] pin: chỉ số pin toàn cục (0..159)
 * @param[in] cb_event: callback (nếu có) để nhận event
 * @return ARM_DRIVER_OK hoặc lỗi
 *
 * Lưu ý: không cấu hình clock, mux, pull ở đây.
 * Chỉ lưu callback, cấu hình logic sẽ do hàm khác đảm nhiệm.
 */
static int32_t GPIO_Setup (ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event) {
  if (pin >= GPIO_MAX_PINS) return ARM_GPIO_ERROR_PIN;
  gpio_cb[pin] = cb_event;
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình hướng (input/output) cho pin
 * @param[in] pin: chỉ số pin toàn cục
 * @param[in] direction: ARM_GPIO_INPUT hoặc ARM_GPIO_OUTPUT
 * @return ARM_DRIVER_OK hoặc lỗi
 */
static int32_t GPIO_SetDirection (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
  if (pin >= GPIO_MAX_PINS) return ARM_GPIO_ERROR_PIN;

  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  if (direction == ARM_GPIO_OUTPUT) {
    GPIO_BASE[port]->PDDR |= (1UL << index);   /* Set pin as output */
  } else {
    GPIO_BASE[port]->PDDR &= ~(1UL << index);  /* Set pin as input */
  }
  return ARM_DRIVER_OK;
}

/**
 * @brief Thiết lập mode output (Push-pull / Open-drain)
 * @note Placeholder: S32K144 không hỗ trợ qua thanh ghi GPIO, sẽ được PORT xử lý.
 */
static int32_t GPIO_SetOutputMode (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
  (void)pin;
  (void)mode;
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình pull-up/pull-down cho pin
 * @note Placeholder: chức năng này thuộc PORT, không xử lý tại GPIO.
 */
static int32_t GPIO_SetPullResistor (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor) {
  (void)pin;
  (void)resistor;
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình trigger event cho pin
 * @note Placeholder: trigger config (IRQC) nằm ở PORT, không xử lý tại GPIO.
 */
static int32_t GPIO_SetEventTrigger (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
  (void)pin;
  (void)trigger;
  return ARM_DRIVER_OK;
}

/**
 * @brief Ghi giá trị ra pin (output)
 * @param[in] pin: chỉ số pin
 * @param[in] val: 0 = low, 1 = high
 */
static void GPIO_SetOutput (ARM_GPIO_Pin_t pin, uint32_t val) {
  if (pin >= GPIO_MAX_PINS) return;

  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  if (val) {
    GPIO_BASE[port]->PSOR = (1UL << index);  /* Set pin */
  } else {
    GPIO_BASE[port]->PCOR = (1UL << index);  /* Clear pin */
  }
}

/**
 * @brief Đọc trạng thái hiện tại của pin (input)
 * @param[in] pin: chỉ số pin
 * @return 0 hoặc 1 (low/high)
 */
static uint32_t GPIO_GetInput (ARM_GPIO_Pin_t pin) {
  if (pin >= GPIO_MAX_PINS) return 0;

  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  return (GPIO_BASE[port]->PDIR >> index) & 1U;
}

/*
 * Driver struct: ánh xạ các hàm cục bộ vào cấu trúc driver
 * để cung cấp API chuẩn CMSIS cho user.
 */
ARM_DRIVER_GPIO Driver_GPIO0 = {
  GPIO_Setup,
  GPIO_SetDirection,
  GPIO_SetOutputMode,
  GPIO_SetPullResistor,
  GPIO_SetEventTrigger,
  GPIO_SetOutput,
  GPIO_GetInput
};
