/*
 * Driver_NVIC.c
 *
 *  Created on: 25 thg 9, 2025
 *      Author: nguyen sy hung
 *
 *
 *  Triển khai các hàm hàm cho NVIC.
 */


#include "Driver_NVIC.h"
/**
 * @brief Enable một IRQ trong NVIC
 */
void NVIC_EnableIRQ(IRQn_Type IRQn) {
    NVIC->ISER[ ((uint32_t)IRQn) >> 5U ] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FU));
}

/**
 * @brief Disable một IRQ trong NVIC
 */
void NVIC_DisableIRQ(IRQn_Type IRQn) {
    NVIC->ICER[ ((uint32_t)IRQn) >> 5U ] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FU));
}

/**
 * @brief Get pending cho một IRQ
 */
uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
    return (NVIC->ISPR[ ((uint32_t)IRQn) >> 5U ] & (1UL << ((uint32_t)IRQn & 0x1FU))) ? 1U : 0U;
}

/**
 * @brief Set pending cho một IRQ
 */
void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
    NVIC->ISPR[ ((uint32_t)IRQn) >> 5U ] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FU));
}


/**
 * @brief Clear pending cho một IRQ
 */
void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
    NVIC->ICPR[ ((uint32_t)IRQn) >> 5U ] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FU));
}

/**
 * @brief Set priority cho một IRQ
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
    NVIC->IP[(uint32_t)IRQn] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
}


/**
 * @brief Get priority cho một IRQ
 */
uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
    return (uint32_t)(NVIC->IP[(uint32_t)IRQn] >> (8U - __NVIC_PRIO_BITS));
}
