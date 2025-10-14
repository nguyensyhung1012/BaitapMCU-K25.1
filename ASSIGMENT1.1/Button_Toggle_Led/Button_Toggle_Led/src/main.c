/*
 * main.c – Test tầng Driver_GPIO (chuẩn CMSIS)
 * Chức năng:
 *  - BT1 (PTC13): Toggle LED đỏ (PTD15)
 *  - BT2 (PTC12): Toggle LED xanh lá (PTD16)
 */

#include "Driver_GPIO.h"
#include "Driver_PORT_S32K144.h"
#include "Driver_GPIO_Pins.h"
#include "S32K144.h"

extern ARM_DRIVER_GPIO Driver_GPIO0;
extern ARM_DRIVER_PORT Driver_PORT0;

/* Callback xử lý nút nhấn */
void gpio_event_callback(ARM_GPIO_Pin_t pin, uint32_t event)
{
    static uint8_t red = 0, green = 0;

    if (event == ARM_GPIO_EVENT_FALLING_EDGE) {
        if (pin == GPIO_PIN_BT1) {
            red ^= 1;
            Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, red);
        } else if (pin == GPIO_PIN_BT2) {
            green ^= 1;
            Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, green);
        }
    }
}

int main(void)
{
    /* Bật clock cho PORTC & PORTD */
    Driver_PORT0.EnableClock(PORTC_INDEX);
    Driver_PORT0.EnableClock(PORTD_INDEX);

    /* Thiết lập MUX */
    Driver_PORT0.SetMux(GPIO_PIN_LED_RED, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_GREEN, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_BT1, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_BT2, ARM_PORT_MUX_GPIO);

    /* Cấu hình LED */
    Driver_GPIO0.Setup(GPIO_PIN_LED_RED, NULL);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_RED, ARM_GPIO_OUTPUT);

    Driver_GPIO0.Setup(GPIO_PIN_LED_GREEN, NULL);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_GREEN, ARM_GPIO_OUTPUT);

    /* Cấu hình Button (input + callback) */
    Driver_GPIO0.Setup(GPIO_PIN_BT1, gpio_event_callback);
    Driver_GPIO0.SetDirection(GPIO_PIN_BT1, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(GPIO_PIN_BT1, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(GPIO_PIN_BT1, ARM_GPIO_TRIGGER_FALLING_EDGE);

    Driver_GPIO0.Setup(GPIO_PIN_BT2, gpio_event_callback);
    Driver_GPIO0.SetDirection(GPIO_PIN_BT2, ARM_GPIO_INPUT);
    Driver_GPIO0.SetPullResistor(GPIO_PIN_BT2, ARM_GPIO_PULL_UP);
    Driver_GPIO0.SetEventTrigger(GPIO_PIN_BT2, ARM_GPIO_TRIGGER_FALLING_EDGE);

    while (1) {
         // chờ ngắt
    }
}
