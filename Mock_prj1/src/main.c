
/**
 * @file main.c
 * @author: Nguyen Sy Hung
 * @brief UART bootloader with SREC -> Flash programming for S32K144
 *
 * This bootloader performs the following operations:
 * - Determines boot mode (BOOTLOADER or USER APP) via button press
 * - Receives SREC format file over UART and parses records
 * - Erases and programs Flash memory using access-code protection
 * - Handles 8-byte aligned Flash programming 
 * - Jumps to USER APP after successful programming or on button release
 *
 */

#include "Driver_USART.h"
#include "uart_buffer.h"
#include "srec_queue.h"
#include "SREC_parser.h"
#include "hal_usart.h"
#include "clock_and_mode.h"
#include "FLASH.h"
#include "S32K144_features.h"
#include "Driver_PORT_S32K144.h"
#include "Driver_GPIO.h"
#include "Driver_GPIO_Pins.h"
#include "s32_core_cm4.h"
#include "cmsis_gcc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* USER APP FLASH REGION (from user linker) */
#define APP_FLASH_START    0x0000A000U   
#define APP_FLASH_LENGTH   0x00076000U   
#define APP_SECTOR_COUNT   (APP_FLASH_LENGTH / FTFC_P_FLASH_SECTOR_SIZE)  
#define APP_FLASH_END      (APP_FLASH_START + APP_FLASH_LENGTH - 1U)      

#define UART_DRIVER        Driver_USART1 
#define MAX_LINE_LENGTH    256U          
#define FLASH_ALIGN_SIZE   8U            /* Flash programming alignment requirement (8 bytes) */
#define FLASH_HALF_SIZE    4U            /* Half of Flash alignment size for 4+4 merge */

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
/**
 * @brief Function pointer type for user application entry point
 */
typedef void (*app_entry_t)(void);

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/* UART DRIVER instances */
extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_GPIO  Driver_GPIO0;
extern ARM_DRIVER_PORT  Driver_PORT0;

/* UART reception buffer */
static uint8_t rx_byte;                     /**< Single byte buffer for UART reception */

/* SREC line assembly buffer */
static char line_buf[MAX_LINE_LENGTH];      /**< Buffer for assembling one SREC line */
static uint16_t line_pos = 0U;              /**< Current position in line buffer */

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/
static inline void UART_SendFast(const char *s);
static void jump_to_app(void);
static void Board_Init(void);
static inline uint8_t Button_Pressed(void);
static void UART_Init(void);
static void Bootloader_Mode(void);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Sends a string over UART1 in blocking mode
 * 
 * This function transmits a null-terminated string via UART and waits
 * until transmission is complete before returning.
 * 
 * @param[in] s Pointer to null-terminated string to send
 *
 */
static inline void UART_SendFast(const char *s)
{
    UART_DRIVER.Send(s, strlen(s));
    while (UART_DRIVER.GetStatus().tx_busy) {
        /* Wait for transmission complete */
    }
}

/**
 * @brief Validates and jumps to user application
 * 
 * This function performs the following steps:
 * 1. Validates user application by checking MSP and Reset Handler
 * 2. Updates VTOR to point to application vector table
 * 3. Sets Main Stack Pointer (MSP) and Process Stack Pointer (PSP)
 * 4. Jumps to application Reset Handler
 * 
 */
static void jump_to_app(void)
{
    UART_SendFast("[BOOT] Jumping to APP...\r\n");

    /* Read MSP and Reset Handler from application vector table */
    uint32_t app_msp   = *(uint32_t *)APP_FLASH_START;
    uint32_t app_reset = *(uint32_t *)(APP_FLASH_START + 4U);

    /* Validate application exists (not erased Flash) */
    if ((app_msp == 0xFFFFFFFFU) || (app_reset == 0xFFFFFFFFU)) {
        UART_SendFast("[BOOT] ERROR: APP not valid!\r\n");
        return;
    }

    /* Update Vector Table Offset Register to application base */
    S32_SCB->VTOR = APP_FLASH_START;

    /* Set stack pointers to application's initial MSP value */
    __set_MSP(app_msp);
    __set_PSP(app_msp);
    
    /* Jump to application Reset Handler */
    app_entry_t app = (app_entry_t)app_reset;
    app();
}

/**
 * @brief Initializes board peripherals (GPIOs for LEDs and button)
 * 
 * Configures the following GPIO pins:
 * - RGB LEDs (RED, GREEN, BLUE) as outputs (initially OFF)
 * - Button (SW2/PC13) as input with pull-up resistor
 * 
 */
static void Board_Init(void)
{
    /* Enable PORT clocks */
    Driver_PORT0.EnableClock(PORTA_INDEX);
    Driver_PORT0.EnableClock(PORTC_INDEX);
    Driver_PORT0.EnableClock(PORTD_INDEX);

    /* Configure LED pins as GPIO */
    Driver_PORT0.SetMux(GPIO_PIN_LED_RED,   ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_GREEN, ARM_PORT_MUX_GPIO);
    Driver_PORT0.SetMux(GPIO_PIN_LED_BLUE,  ARM_PORT_MUX_GPIO);

    /* Set LED pins as outputs */
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_RED,   ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_GREEN, ARM_GPIO_OUTPUT);
    Driver_GPIO0.SetDirection(GPIO_PIN_LED_BLUE,  ARM_GPIO_OUTPUT);

    /* Configure button pin as GPIO with pull-up */
    Driver_PORT0.SetMux(GPIO_PIN_BT1, ARM_PORT_MUX_GPIO);
    Driver_GPIO0.SetPullResistor(GPIO_PIN_BT1, ARM_PORT_PULL_UP);
    Driver_GPIO0.SetDirection(GPIO_PIN_BT1, ARM_GPIO_INPUT);

    /* Turn off all LEDs initially */
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_RED,   0);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_GREEN, 0);
    Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE,  0);
}

/**
 * @brief Checks if boot button is pressed
 * 
 * Reads the state of the boot button (active LOW).
 * 
 * @return uint8_t Button state
 * @retval 1 Button is pressed (pin LOW)
 * @retval 0 Button is not pressed (pin HIGH)
 */
static inline uint8_t Button_Pressed(void)
{
    if (Driver_GPIO0.GetInput(GPIO_PIN_BT1) == 0U) {
        return 1U;
    }
    return 0U;
}

/**
 * @brief UART interrupt callback handler
 * 
 * This callback is invoked by the UART driver on completion events.
 * When a byte is received, it:
 * 1. Pushes the received byte to the circular buffer
 * 2. Re-arms the UART receiver for the next byte
 * 
 * @param[in] event UART event flags from ARM CMSIS Driver
 */
void UART_EventHandler(uint32_t event)
{
    if ((event & ARM_USART_EVENT_RECEIVE_COMPLETE) != 0U) {
        (void)UART_BufferPush(rx_byte);
        (void)UART_DRIVER.Receive(&rx_byte, 1U);
    }
}

/**
 * @brief Initializes UART1 peripheral for bootloader communication
 * 
 * Configures UART1 with the following parameters:
 * - Baud rate: 9600 bps
 * - Data bits: 8
 * - Parity: None
 * - Stop bits: 1
 * - Mode: Asynchronous
 * 
 * Also initializes:
 * - UART circular buffer for byte reception
 * - SREC queue for line storage
 * 
 */
static void UART_Init(void)
{
    /* Initialize buffers */
    UART_BufferInit();
    SREC_QueueInit();

    /* Initialize UART driver */
    UART_DRIVER.Initialize(UART_EventHandler);
    UART_DRIVER.Control(ARM_USART_MODE_ASYNCHRONOUS |
                        ARM_USART_DATA_BITS_8       |
                        ARM_USART_PARITY_NONE       |
                        ARM_USART_STOP_BITS_1,
                        HAL_USART_BAUDRATE_9600);

    /* Start receiving first byte */
    UART_DRIVER.Receive(&rx_byte, 1U);
}

/**
 * @brief Main bootloader state machine for SREC reception and Flash programming
 * 
 * This function performs two main tasks:
 * 
 * 1. UART Reception:
 *    - Assembles received bytes into complete SREC lines (terminated by '\n')
 *    - Pushes valid SREC lines (starting with 'S') to the queue
 * 
 * 2. SREC Processing and Flash Programming:
 *    - Parses queued SREC records (S1/S2/S3 data records)
 *    - Implements 8-byte aligned Flash programming with 4+4 byte merging strategy:
 *      a) Exactly 4 bytes at offset 0: Store as pending
 *      b) 4 bytes at offset +4: Merge with pending and program 8 bytes
 *      c) 8 or more bytes aligned: Program directly
 *      d) Unaligned data: Pad with 0xFF and program
 *    - Flushes pending data on end-of-file records (S7/S8/S9)
 * 
 * @note This function should be called continuously in the main loop
 * @note Flash programming is performed with interrupts disabled
 * @note Addresses must be within APP_FLASH_START to APP_FLASH_END range
 * 
 * @warning Flash must be erased before programming
 * @warning Do not call this function with interrupts disabled
 */
static void Bootloader_Mode(void)
{
    uint8_t c;
    char srec_line[MAX_LINE_LENGTH];

    /* State for 4+4 byte merge strategy (static to persist across calls) */
    static uint8_t  pending_valid = 0U;      /* Flag: pending low 4 bytes available */
    static uint32_t pending_base  = 0U;      /* Aligned base address of pending data */
    static uint8_t  pending_low4[FLASH_HALF_SIZE]; /* Pending low 4 bytes buffer */

    /* Working variables for parsing and Flash operations */
    srec_record_t rec;
    uint32_t addr;
    uint32_t len;
    uint8_t *p;
    uint32_t base;
    uint32_t off;
    uint32_t n;
    uint8_t  buf8[FLASH_ALIGN_SIZE];

    /* ==================== UART Byte Reception -> Line Assembly ==================== */
    if (UART_BufferPop(&c)) {
        if (c == '\n') {
            /* End of line detected */
            if (line_pos > 0U) {
                line_buf[line_pos] = '\0';

                /* Only queue valid SREC lines (starting with 'S') */
                if (line_buf[0] == 'S') {
                    (void)SREC_QueuePush(line_buf);
                }

                line_pos = 0U;
            }
        } else {
            /* Append character to line buffer */
            if (line_pos < (sizeof(line_buf) - 1U)) {
                line_buf[line_pos++] = (char)c;
            } else {
                /* Buffer overflow, reset */
                line_pos = 0U;
            }
        }
    }

    /* ==================== SREC Processing -> Parse -> Flash Programming ==================== */
    while (SREC_QueuePop(srec_line)) {
        if ((parse_srec_line(srec_line, &rec) == 0) && (rec.valid != 0)) {
            
            /* ---------- Process Data Records (S1/S2/S3) ---------- */
            if ((rec.type == 1) || (rec.type == 2) || (rec.type == 3)) {
                
                /* Verify address is within application Flash range */
                if ((rec.address >= APP_FLASH_START) && (rec.address <= APP_FLASH_END)) {
                    addr = rec.address;
                    len  = rec.data_len;
                    p    = rec.data;

                    /* Process all bytes in this record */
                    while (len > 0U) {
                        
                        /* Case 1: Exactly 4 bytes at 8-byte aligned offset 0 -> Store as pending */
                        if (((addr & 0x7U) == 0U) && (len == 4U)) {
                            memcpy(pending_low4, p, FLASH_HALF_SIZE);
                            pending_base  = addr;
                            pending_valid = 1U;

                            addr += FLASH_HALF_SIZE;
                            p    += FLASH_HALF_SIZE;
                            len  -= FLASH_HALF_SIZE;
                        }
                        
                        /* Case 2: At least 4 bytes at offset +4 -> Merge with pending and program */
                        else if (((addr & 0x7U) == 4U) && (len >= FLASH_HALF_SIZE)) {
                            base = addr - FLASH_HALF_SIZE;  /* Get aligned base address */
                            memset(buf8, 0xFF, FLASH_ALIGN_SIZE);

                            /* If we have matching pending data, use it */
                            if ((pending_valid != 0U) && (pending_base == base)) {
                                memcpy(buf8, pending_low4, FLASH_HALF_SIZE);
                            }

                            /* Copy high 4 bytes */
                            memcpy(&buf8[FLASH_HALF_SIZE], p, FLASH_HALF_SIZE);

                            /* Program 8 bytes with interrupts disabled */
                            DISABLE_INTERRUPTS();
                            (void)Program_LongWord_8B(base, buf8);
                            ENABLE_INTERRUPTS();

                            pending_valid = 0U;

                            addr += FLASH_HALF_SIZE;
                            p    += FLASH_HALF_SIZE;
                            len  -= FLASH_HALF_SIZE;
                        }
                        
                        /* Case 3: At least 8 bytes at aligned offset -> Program directly */
                        else if (((addr & 0x7U) == 0U) && (len >= FLASH_ALIGN_SIZE)) {
                            DISABLE_INTERRUPTS();
                            (void)Program_LongWord_8B(addr, p);
                            ENABLE_INTERRUPTS();

                            addr += FLASH_ALIGN_SIZE;
                            p    += FLASH_ALIGN_SIZE;
                            len  -= FLASH_ALIGN_SIZE;
                        }else{
                        	/* do nothing */
                        }
                    }
                }
            }

            /* ---------- Process End-of-File Records (S7/S8/S9) ---------- */
            if ((rec.type == 7) || (rec.type == 8) || (rec.type == 9)) {
                
                /* Flush any pending 4-byte data */
                if (pending_valid != 0U) {
                    memset(buf8, 0xFF, FLASH_ALIGN_SIZE);
                    memcpy(buf8, pending_low4, FLASH_HALF_SIZE);

                    DISABLE_INTERRUPTS();
                    (void)Program_LongWord_8B(pending_base, buf8);
                    ENABLE_INTERRUPTS();

                    pending_valid = 0U;
                }

                /* Notify completion */
                UART_SendFast("\r\n[INFO] Flash programming completed.\r\n");
                UART_SendFast("[INFO] Please RESET the board WITHOUT pressing the BOOT button.\r\n");
                UART_SendFast("[INFO] The new application will run after reset.\r\n");

                /* Reset state for next programming session */
                SREC_QueueInit();
                line_pos = 0U;
            }
        }
    }
}

/**
 * @brief Main entry point of bootloader
 * 
 * Execution flow:
 * 1. Initialize system clocks (SOSC 8MHz, SPLL 160MHz, core 80MHz)
 * 2. Initialize UART and GPIO peripherals
 * 3. Check boot button state:
 *    - If NOT pressed: Jump to user application
 *    - If pressed: Enter bootloader mode
 * 4. In bootloader mode:
 *    - Turn on BLUE LED indicator
 *    - Load Flash access code
 *    - Erase application Flash region
 *    - Enter infinite loop processing SREC data
 * 
 * @return int Never returns (infinite loop or jumps to application)
 * 
 * @note System reset is required to exit bootloader mode after programming
 */
int main(void)
{
    /* Initialize system clocks */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    /* Initialize peripherals */
    UART_Init();
    Board_Init();

    UART_SendFast("BOOT READY\n");

    /* Check boot mode selection */
    if (Button_Pressed() == 0U) {
        /* Button not pressed -> Jump to application */
        UART_SendFast("Button not pressed\n");
        jump_to_app();
    } else {
        /* Button pressed -> Enter bootloader mode */
        Driver_GPIO0.SetOutput(GPIO_PIN_LED_BLUE, 1);
        UART_SendFast("[BOOT] Please send USER APP SREC file...\r\n");

        /* Load Flash access code and erase application region */
        Mem_43_INFLS_IPW_LoadAc();
        DISABLE_INTERRUPTS();
        Erase_Multi_Sector(APP_FLASH_START, APP_SECTOR_COUNT);
        ENABLE_INTERRUPTS();

        UART_SendFast("[FLASH] Ready\r\n");
    }

    /* Main bootloader loop - process SREC data continuously */
    while (1) {
        Bootloader_Mode();
    }

    return 0;
}
