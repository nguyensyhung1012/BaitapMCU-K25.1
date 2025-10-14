/*
 * Driver_GPIO_Pins.h
 *
 *  Created on: 25 thg 9, 2025
 *      Author: nguye
 *
 *  Định nghĩa macro cho các chân (pin) trên board S32K144.
 */

#ifndef DRIVER_GPIO_PINS_H_
#define DRIVER_GPIO_PINS_H_

/*
 * File: GPIO_Pins.h
 * Mục đích:
 *   - Gom tất cả define cho pin GPIO: LED, nút nhấn, cổng, index...
 *   - Dùng chung cho tầng HAL và Driver
 *
 * Board: NXP S32K144 EVB
 * Author: ChatGPT - 2025
 */

/* ==== Định nghĩa tổng số pin toàn chip ==== */
#define GPIO_TOTAL_PINS     (160U)   // 5 port * 32 pin mỗi port

/* ==== Macro hỗ trợ ==== */
#define GPIO_PIN_ID(port, pin)    ((port) * 32U + (pin))
#define GPIO_PIN_PORT(id)         ((id) / 32U)
#define GPIO_PIN_INDEX(id)        ((id) % 32U)
#define GPIO_IS_VALID(pin)        ((pin) < GPIO_TOTAL_PINS)

/* ==== Danh sách cổng ==== */
typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E
} GPIO_Port_t;

/* ==== Mapping LED và nút nhấn trên S32K144 EVB ==== */

/* LED */
#define GPIO_LED_RED_PORT         GPIO_PORT_D
#define GPIO_LED_RED_PIN          15U
#define GPIO_LED_GREEN_PORT       GPIO_PORT_D
#define GPIO_LED_GREEN_PIN        16U
#define GPIO_LED_BLUE_PORT        GPIO_PORT_D
#define GPIO_LED_BLUE_PIN         0U

/* BUTTON (Switch) */
#define GPIO_BT1_PORT             GPIO_PORT_C
#define GPIO_BT1_PIN              13U
#define GPIO_BT2_PORT             GPIO_PORT_C
#define GPIO_BT2_PIN              12U

/* ==== Define dạng global pin ID (cho CMSIS Driver) ==== */
#define GPIO_PIN_LED_RED          GPIO_PIN_ID(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN)
#define GPIO_PIN_LED_GREEN        GPIO_PIN_ID(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN)
#define GPIO_PIN_LED_BLUE         GPIO_PIN_ID(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN)

#define GPIO_PIN_BT1              GPIO_PIN_ID(GPIO_BT1_PORT, GPIO_BT1_PIN)
#define GPIO_PIN_BT2              GPIO_PIN_ID(GPIO_BT2_PORT, GPIO_BT2_PIN)

/* ==== Index cho PORT driver (nếu dùng Driver_PORT_S32K144) ==== */
#define PORTA_INDEX   (0U)
#define PORTB_INDEX   (1U)
#define PORTC_INDEX   (2U)
#define PORTD_INDEX   (3U)
#define PORTE_INDEX   (4U)



#endif /* DRIVER_GPIO_PINS_H_ */
