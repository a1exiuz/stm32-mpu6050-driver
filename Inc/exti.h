#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>

#define EXTI ((EXTI_RegMap_t*)0x40013C00UL)
#define SYSCFG ((SYSCFG_RegMap_t*)0x40013800UL)

#define EXTI0_IRQn       6
#define EXTI1_IRQn       7
#define EXTI2_IRQn       8
#define EXTI3_IRQn       9
#define EXTI4_IRQn       10
#define EXTI9_5_IRQn     23
#define EXTI15_10_IRQn   40

typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t RESERVED1[1];
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t RESERVED2[1];
    volatile uint32_t PR;
} EXTI_RegMap_t;

typedef struct {
    volatile uint32_t RESERVED2[2];
    volatile uint32_t EXTICR[4];
    volatile uint32_t RESERVED3[2];
} SYSCFG_RegMap_t;

void EXTI_init(uint8_t pin, uint8_t port_code, uint8_t priority);

void EXTI0_IRQHandler(void);

void EXTI4_IRQHandler(void);

void EXTI9_5_IRQHandler(void);

uint8_t EXTI_get_flag1(void);

uint8_t EXTI_get_flag2(void);

uint8_t EXTI_get_flag3(void);
/*
EXTI0_IRQHandler()      // pin 0
EXTI1_IRQHandler()      // pin 1
EXTI2_IRQHandler()      // pin 2
EXTI3_IRQHandler()      // pin 3
EXTI4_IRQHandler()      // pin 4
EXTI9_5_IRQHandler()    // pins 5-9  (shared)
EXTI15_10_IRQHandler()  // pins 10-15 (shared)

#define EXTI0_IRQn       6
#define EXTI1_IRQn       7
#define EXTI2_IRQn       8
#define EXTI3_IRQn       9
#define EXTI4_IRQn       10
#define EXTI9_5_IRQn     23
#define EXTI15_10_IRQn   40
*/
#endif
