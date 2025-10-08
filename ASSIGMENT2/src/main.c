/**
 * @file main.c
 * @author nguyen sy hung (nguyensyhung10122001@gmail.com)
 * @brief su dung USART de truyen nhan du lieu, thay doi trang thai led
 * @version 0.1
 * @date 2025-10-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "S32K144.h"
#include "clock_and_mode.h"
#include "Driver_GPIO.h"
#include "Driver_PORT_S32K144.h"
#include "Driver_GPIO_Pins.h"
#include "Driver_NVIC.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <string.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_CMD_LEN 50
/*******************************************************************************
 * Variables
 ******************************************************************************/
extern ARM_DRIVER_GPIO Driver_GPIO0;
extern ARM_DRIVER_PORT Driver_PORT0;
extern ARM_DRIVER_USART Driver_USART1;
extern uint8_t rx_data; 

volatile char rx_buffer[MAX_CMD_LEN];
volatile uint32_t rx_index = 0;
volatile uint8_t rx_done = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*==== Led init ====*/
void LED_Init(void)
{
    Driver_PORT0.EnableClock(3);

    Driver_PORT0.SetMux(GPIO_PIN_LED_RED, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_GREEN, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_BLUE, ARM_PORT_MUX_GPIO);

    Driver_GPIO0.SetDirection(GPIO_PIN_LED_RED, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_GREEN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_BLUE, ARM_GPIO_OUTPUT);

    Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, 1);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 1);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 1);
}
/*==== LPIT0 init ====*/
void LPIT0_init(void)
{
    IP_PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(7);
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
    IP_LPIT0->MCR = LPIT_MCR_M_CEN_MASK;
    IP_LPIT0->TMR[0].TVAL = 25600;
    IP_LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_MODE(1);
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
}
/*==== ISR LPIT0 ====*/
void LPIT0_Ch0_IRQHandler(void)
{
    if (IP_LPIT0->MSR & LPIT_MSR_TIF0_MASK)
    {
        IP_LPIT0->MSR = LPIT_MSR_TIF0_MASK;
        rx_buffer[rx_index] = '\0';
        rx_done = 1;
        IP_LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK;
    }
}
/* Callback khi nhận dữ liệu xong, lưu ký tự vào buffer, 
sử dụng timmer để xác nhận nhập xong chuỗi */
void USART1_Event(uint32_t event)
{
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        /* Lưu ký tự vào buffer */
        if (rx_index < MAX_CMD_LEN - 1)
            rx_buffer[rx_index++] = rx_data;

        /* Reset timer để phát hiện timeout chuỗi */
        IP_LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK;
        IP_LPIT0->TMR[0].TVAL = 6400;
        IP_LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_MODE(1) | LPIT_TMR_TCTRL_T_EN_MASK;
    }
}
/* hàm xử lý tín hiệu led, đưa thông tin ra hercules */
void ProcessCommand(void)
{
    const char *cmd = (const char *)rx_buffer;

    if (strcmp(cmd, "LED STATUS") == 0)
    {
        char buf[64];
        sprintf(buf, "RED=%ld, GREEN=%ld, BLUE=%ld\r\n",
                Driver_GPIO0.GetInput(GPIO_PIN_LED_RED),
                Driver_GPIO0.GetInput(GPIO_PIN_LED_GREEN),
                Driver_GPIO0.GetInput(GPIO_PIN_LED_BLUE));
        Driver_USART1.Send(buf, strlen(buf));
    }
    else if (strcmp(cmd, "RED ON") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, 0);
    else if (strcmp(cmd, "RED OFF") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED, 1);
    else if (strcmp(cmd, "GREEN ON") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 0);
    else if (strcmp(cmd, "GREEN OFF") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 1);
    else if (strcmp(cmd, "BLUE ON") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 0);
    else if (strcmp(cmd, "BLUE OFF") == 0) Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 1);
    else if (strcmp(cmd, "HELP") == 0)
    {
        Driver_USART1.Send("Commands:\r\n LED STATUS\r\n RED ON/OFF\r\n GREEN ON/OFF\r\n BLUE ON/OFF\r\n HELP\r\n",
                           strlen("Commands:\r\n LED STATUS\r\n RED ON/OFF\r\n GREEN ON/OFF\r\n BLUE ON/OFF\r\n HELP\r\n"));
    }
    else
    {
        Driver_USART1.Send("Command not available\r\n", strlen("Command not available\r\n"));
    }
}

int main(void)
{
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    LED_Init();
    LPIT0_init();

    /*==== Init UART CMSIS ====*/
    Driver_USART1.Initialize(USART1_Event);
    USART1_SetClockSource(1);
    Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1,
                          9600);

    Driver_USART1.Send("Running CMSIS USART Example\r\n> ", strlen("Running CMSIS USART Example\r\n> "));

    while (1)
    {
    	/* báo nhận xong dữ liệu */
        if (rx_done)
        {
            rx_done = 0;
            /* trả kết quả đã nhận ra ra màn hình */
            Driver_USART1.Send("\r\nReceived: ", 12);
            Driver_USART1.Send((void *)rx_buffer, strlen((char *)rx_buffer));
            Driver_USART1.Send("\r\n", 2);
            /* thực hiện xử lý led */
            ProcessCommand();
            /* reset lại buffer */
            rx_index = 0;
            Driver_USART1.Send("\r\n> ", 4);
        }
    }
}
