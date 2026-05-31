/*
    exti.h
    External Interrupt (EXTI) driver for STM32F446RE
    Configures GPIO pins as interrupt sources with edge detection
    Requires SYSCFG to map GPIO port to EXTI line
    Base addresses:
        EXTI: 0x40013C00
        SYSCFG: 0x40013800
    Reference: STM32F446RE Reference Manual RM0390, Section 12
*/
#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>

#define EXTI ((EXTI_RegMap_t*)0x40013C00UL)
#define SYSCFG ((SYSCFG_RegMap_t*)0x40013800UL)

// IRQ numbers for EXTI lines - used by NVIC to enable correct interrupt
#define EXTI0_IRQn       6      //pin 0 - dedicated IRQ
#define EXTI1_IRQn       7      //pin 1 - dedicated IRQ
#define EXTI2_IRQn       8      //pin 2 - dedicated IRQ
#define EXTI3_IRQn       9      //pin 3 - dedicated IRQ
#define EXTI4_IRQn       10     //pin 4 - dedicated IRQ
#define EXTI9_5_IRQn     23     //pins 5-9 - shared IRQ, check PR in handler
#define EXTI15_10_IRQn   40     //pins 10-15 - shared IRQ, check PR in handler

/*
    EXTI register map
    Controls interrupt masking, edge detection, and pending flags
*/
typedef struct {
    volatile uint32_t IMR;          //0x00 - interrupt mask (1=unmasked/enabled)
    volatile uint32_t RESERVED1[1]; //0x04 - reserved (EMR even mask, not used)
    volatile uint32_t RTSR;         //0x08 - rising edge trigger select
    volatile uint32_t FTSR;         //0x0C - falling edge trigger select
    volatile uint32_t RESERVED2[1]; //0x10 - resereved (SWIER software interrupt)
    volatile uint32_t PR;           //0x14 - pending register (write 1 to clear)
} EXTI_RegMap_t;

/*
    SYSCFG register map (partial)
    EXTICR registers map GPIO ports to EXTI lines
    Only EXTICR fields included - other SYSCFG registers not used
*/
typedef struct {
    volatile uint32_t RESERVED2[2]; // 0x00-0x04 - MEMRMP, PMC (not used)
    volatile uint32_t EXTICR[4];    // 0x08-0x14 - EXTI port selection (4 bits per pin)
    volatile uint32_t RESERVED3[2]; // 0x18-0x1C - reserved
} SYSCFG_RegMap_t;

/*
    EXTI_init
    Configures a GPIO pin as a falling edge triggered external interrupt
    Enables SYSCFG clock, maps port to EXTI line, and enables via NVIC
    pin:    GPIO pin number 0-15
    port_code: 0=GPIOA, 1=GPIOB, 2=GPIOC (written to SYSCFG EXTICR)
    priority: NVIC priority 0=highest, 15=lowest
    Ex:
        EXTI_init(0, 0 , 1); // PA0, falling edge, pririoty 1
*/
void EXTI_init(uint8_t pin, uint8_t port_code, uint8_t priority);

/*
    IRQ Handlers - called automatically by hardware on interrupt
    Check PR register to indentify which pin triggered (shared handlers)
    Clear PR flag by writing 1 to the corresponding bit
    Do not call directly
*/
void EXTI0_IRQHandler(void);    // pin 0

void EXTI4_IRQHandler(void);    // pin 4

void EXTI9_5_IRQHandler(void);  //pin 5-9 shared

/*
    EXTI_get_flag 1/2/3
    Returns 1 if the corresponding button interrupt has fired, 0 otherwise
    Clears the flag on read - call once per loop iteration
    flag1: pin 7 (EXTI9_5)
    flag2: pin 4 (EXTI4)
    flag3: pin 0 (EXTI0)
*/
uint8_t EXTI_get_flag1(void);

uint8_t EXTI_get_flag2(void);

uint8_t EXTI_get_flag3(void);

#endif
