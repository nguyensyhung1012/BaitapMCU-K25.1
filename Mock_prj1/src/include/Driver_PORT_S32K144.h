/*
 * Driver_PORT_S32K144.h
 *
 * Created on: 25 thg 9, 2025
 *      Author: nguyen sy hung
 *
 * Driver quản lý PORT module của S32K144 theo chuẩn CMSIS-style.
 * - PORT chịu trách nhiệm về clock gating, mux (chọn chức năng pin),
 *   pull-up/down và cấu hình interrupt trigger (IRQC).
 * - Các thao tác logic (set/clear/toggle, input/output) được tách sang Driver_GPIO.
 */

#ifndef DRIVER_PORT_S32K144_H_
#define DRIVER_PORT_S32K144_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Driver_Common.h"
#include "S32K144.h"

/* Kiểu dữ liệu định danh pin (0..159, tính theo toàn cục) */
typedef uint32_t ARM_PORT_Pin_t;

/* ===== MUX options cho từng chân =====
 * Xác định chân sẽ dùng như GPIO hay chức năng alternate.
 */
typedef enum {
  ARM_PORT_MUX_DISABLED = 0,   /* Pin bị disable (default reset state) */
  ARM_PORT_MUX_GPIO     = 1,   /* Chân hoạt động như GPIO */
  ARM_PORT_MUX_ALT2     = 2,   /* Alternate 2 */
  ARM_PORT_MUX_ALT3     = 3,   /* Alternate 3 */
  ARM_PORT_MUX_ALT4     = 4,   /* Alternate 4 */
  ARM_PORT_MUX_ALT5     = 5,   /* Alternate 5 */
  ARM_PORT_MUX_ALT6     = 6,   /* Alternate 6 */
  ARM_PORT_MUX_ALT7     = 7    /* Alternate 7 */
} ARM_PORT_MUX;

/* ===== Pull resistor options =====
 * Cho phép bật/tắt điện trở kéo lên hoặc kéo xuống trong PORT_PCR.
 */
typedef enum {
  ARM_PORT_PULL_NONE = 0,  /* Không dùng pull resistor */
  ARM_PORT_PULL_UP   = 1,  /* Kéo lên (pull-up) */
  ARM_PORT_PULL_DOWN = 2   /* Kéo xuống (pull-down) */
} ARM_PORT_PULL;

/* ===== Event types =====
 * Sử dụng trong callback khi có interrupt từ PORT.
 */
typedef enum {
  ARM_PORT_EVENT_NONE         = 0, /* Không có sự kiện */
  ARM_PORT_EVENT_RISING_EDGE  = 1, /* Ngắt xảy ra khi cạnh lên */
  ARM_PORT_EVENT_FALLING_EDGE = 2  /* Ngắt xảy ra khi cạnh xuống */
} ARM_PORT_EVENT;

/* Prototype cho hàm callback event */
typedef void (*ARM_PORT_SignalEvent_t)(ARM_PORT_Pin_t pin, ARM_PORT_EVENT event);

/* ===== API Driver PORT =====
 * Cung cấp tập hàm cơ bản để enable clock, chọn mux, cấu hình pull và ngắt.
 */
typedef struct _ARM_DRIVER_PORT {
  int32_t (*EnableClock) (uint32_t port);                             /* Bật clock cho PORTx */
  int32_t (*SetMux)      (ARM_PORT_Pin_t pin, ARM_PORT_MUX mux);      /* Chọn mux cho pin */
  int32_t (*SetPull)     (ARM_PORT_Pin_t pin, ARM_PORT_PULL pull);    /* Cấu hình pull resistor */
  int32_t (*SetInterrupt)(ARM_PORT_Pin_t pin, uint32_t trigger,
                          ARM_PORT_SignalEvent_t cb);                 /* Cấu hình ngắt + callback */
} const ARM_DRIVER_PORT;

/* Driver instance toàn cục (theo CMSIS rule) */
extern ARM_DRIVER_PORT Driver_PORT0;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_PORT_S32K144_H_ */
