/*
 * Driver_PORT_S32K144.c
 *
 *  Created on: 25 thg 9, 2025
 *      Author: nguyen sy hung
 *
 * Triển khai driver cho PORT module của S32K144.
 * - Enable clock cho PORT
 * - Config mux (GPIO, ALT)
 * - Config pull-up/down
 * - Config interrupt trigger (IRQC)
 * - Xử lý ISR và gọi callback
 */

#include "Driver_PORT_S32K144.h"
#include "Driver_NVIC.h"
#include "hal_gpio.h"

#define PORT_MAX_PINS   160U               /* Tổng số pin (5 port * 32 pin) */
#define PIN_PORT(pin)   ((pin) / 32U)      /* Tính số port từ chỉ số pin */
#define PIN_INDEX(pin)  ((pin) % 32U)      /* Tính index trong port */

static PORT_Type * const PORT_BASE[] = IP_PORT_BASE_PTRS;   /* Base address cho PORTA..E */


/* Mảng lưu callback cho từng pin (nếu có interrupt) */
static ARM_PORT_SignalEvent_t port_cb[PORT_MAX_PINS];

/**
 * @brief Enable clock cho PORTx
 * @param[in] port: 0=A, 1=B, 2=C, 3=D, 4=E
 */
static int32_t PORT_EnableClock(uint32_t port) {
  if (port > 4) return ARM_DRIVER_ERROR_PARAMETER;
  IP_PCC->PCCn[PCC_PORTA_INDEX + port] |= PCC_PCCn_CGC_MASK;
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình mux cho pin
 * @param[in] pin: số pin toàn cục (0..159)
 * @param[in] mux: lựa chọn ARM_PORT_MUX_xxx
 */
static int32_t PORT_SetMux(ARM_PORT_Pin_t pin, ARM_PORT_MUX mux) {
  if (pin >= PORT_MAX_PINS) return ARM_DRIVER_ERROR_PARAMETER;
  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  PORT_BASE[port]->PCR[index] &= ~PORT_PCR_MUX_MASK;
  PORT_BASE[port]->PCR[index] |= PORT_PCR_MUX(mux);
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình pull resistor cho pin
 * @param[in] pin: số pin toàn cục
 * @param[in] pull: ARM_PORT_PULL_NONE/UP/DOWN
 */
static int32_t PORT_SetPull(ARM_PORT_Pin_t pin, ARM_PORT_PULL pull) {
  if (pin >= PORT_MAX_PINS) return ARM_DRIVER_ERROR_PARAMETER;
  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  PORT_BASE[port]->PCR[index] &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
  switch (pull) {
    case ARM_PORT_PULL_UP:
      PORT_BASE[port]->PCR[index] |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
      break;
    case ARM_PORT_PULL_DOWN:
      PORT_BASE[port]->PCR[index] |= PORT_PCR_PE_MASK;
      break;
    default:
      break;
  }
  return ARM_DRIVER_OK;
}

/**
 * @brief Cấu hình interrupt trigger cho pin
 * @param[in] pin: số pin toàn cục
 * @param[in] trigger: giá trị IRQC (0..15 theo ref manual)
 * @param[in] cb: hàm callback khi có interrupt
 */
static int32_t PORT_SetInterrupt(ARM_PORT_Pin_t pin, uint32_t trigger, ARM_PORT_SignalEvent_t cb) {
  if (pin >= PORT_MAX_PINS) return ARM_DRIVER_ERROR_PARAMETER;

  uint32_t port = PIN_PORT(pin);
  uint32_t index = PIN_INDEX(pin);

  /* Ghi callback */
  port_cb[pin] = cb;

  /* Cấu hình IRQC */
  PORT_BASE[port]->PCR[index] &= ~PORT_PCR_IRQC_MASK;
  PORT_BASE[port]->PCR[index] |= PORT_PCR_IRQC(trigger);

  /* Enable NVIC cho port tương ứng */
  switch (port) {
    case 0: NVIC_EnableIRQ(PORTA_IRQn); break;
    case 1: NVIC_EnableIRQ(PORTB_IRQn); break;
    case 2: NVIC_EnableIRQ(PORTC_IRQn); break;
    case 3: NVIC_EnableIRQ(PORTD_IRQn); break;
    case 4: NVIC_EnableIRQ(PORTE_IRQn); break;
    default: break;
  }

  return ARM_DRIVER_OK;
}

/**
 * @brief Xử lý ISR chung cho từng port
 * @param[in] port: số port (0..4)
 */


/* ISR cho từng PORT (wrapper) */
void PORTA_IRQHandler(void)
{
    HAL_GPIO_IRQHandler(HAL_GPIO_PORT_A);
}

void PORTB_IRQHandler(void)
{
    HAL_GPIO_IRQHandler(HAL_GPIO_PORT_B);
}

void PORTC_IRQHandler(void)
{
    HAL_GPIO_IRQHandler(HAL_GPIO_PORT_C);
}

void PORTD_IRQHandler(void)
{
    HAL_GPIO_IRQHandler(HAL_GPIO_PORT_D);
}

void PORTE_IRQHandler(void)
{
    HAL_GPIO_IRQHandler(HAL_GPIO_PORT_E);
}

/* Driver struct ánh xạ các hàm */
ARM_DRIVER_PORT Driver_PORT0 = {
  PORT_EnableClock,
  PORT_SetMux,
  PORT_SetPull,
  PORT_SetInterrupt
};

