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

/* Định nghĩa enum cho Port index */
typedef enum {
    PORTA_INDEX = 0,
    PORTB_INDEX = 1,
    PORTC_INDEX = 2,
    PORTD_INDEX = 3,
    PORTE_INDEX = 4
} GPIO_PortIndex;

/* Macro tính toán pin index */
#define GPIO_PIN(port, index)   (((port) * 32U) + (index))

/* ===== Buttons trên BOARD S32K144 ===== */
#define GPIO_PIN_BT1   GPIO_PIN(PORTC_INDEX, 13)   // PTC13 - Button SW3 (BT1)
#define GPIO_PIN_BT2   GPIO_PIN(PORTC_INDEX, 12)   // PTC12 - Button SW2 (BT2)

/* ===== LEDs trên BOARD S32K144 ===== */
#define GPIO_PIN_LED_RED    GPIO_PIN(PORTD_INDEX, 15)   // PTD15 - LED đỏ
#define GPIO_PIN_LED_GREEN  GPIO_PIN(PORTD_INDEX, 16)   // PTD16 - LED xanh lá
#define GPIO_PIN_LED_BLUE   GPIO_PIN(PORTD_INDEX, 0)    // PTD0  - LED xanh dương



#endif /* DRIVER_GPIO_PINS_H_ */
