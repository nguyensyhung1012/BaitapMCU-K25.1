/*
 * Driver_NVIC.h
 *
 *  Created on: 25 thg 9, 2025
 *      Author: nguyen sy hung
 *
 *  Cung cấp API đơn giản để thao tác với NVIC
 */

#ifndef INCLUDE_DRIVER_NVIC_H_
#define INCLUDE_DRIVER_NVIC_H_

#include "S32K144.h"

#ifdef __cplusplus
extern "C" {
#endif
/* System Control Space Base Address */
#define SCS_BASE            (0xE000E000UL)
/* NVIC Base Address */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)


/**
\brief Access structure of the NVIC Driver.
*/
typedef struct {
  volatile uint32_t ISER[8U];              /* Offset: 0x000 (R/W) Interrupt Set Enable Register */
           uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];              /* Offset: 0x080 (R/W) Interrupt Clear Enable Register */
           uint32_t RESERVED1[24U];
  volatile uint32_t ISPR[8U];              /* Offset: 0x100 (R/W) Interrupt Set Pending Register */
           uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];              /* Offset: 0x180 (R/W) Interrupt Clear Pending Register */
           uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];              /* Offset: 0x200 (R/W) Interrupt Active Bit Register */
           uint32_t RESERVED4[56U];
  volatile uint8_t  IP[240U];              /* Offset: 0x300 (R/W) Interrupt Priority Register */
           uint32_t RESERVED5[644U];
  volatile  uint32_t STIR;                 /* Offset: 0xE00 ( /W) Software Trigger Interrupt Register */
} NVIC_Type;

/* Base address cho NVIC */
#define NVIC                ((NVIC_Type*) NVIC_BASE)

/* NVIC API giống CMSIS */
void NVIC_EnableIRQ(IRQn_Type IRQn);
void NVIC_DisableIRQ(IRQn_Type IRQn);
uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn);
void NVIC_SetPendingIRQ(IRQn_Type IRQn);
void NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
uint32_t NVIC_GetPriority(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVER_NVIC_H_ */
