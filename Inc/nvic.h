/*
    nvic.h
    Nested Vector Interrupt Controller (NVIC) driver for STM32F446RE
    Provides IRQ enable, disable, and priority configuration
    NVIC is an ARM Cortex-M4 core peripheral - same on all Cortex - M4 devices
    Base address: 0xE000E100
    Reference: STM32F446RE Reference Manual RM0390, Section 10
                ARM Cortex-M4 Generic User Guide, Section 4.2
*/
#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

// NVIC interrupt set-enable registers - each bit enables one IRQ
#define NVIC_ISER0 (*(volatile uint32_t*)0xE000E100UL) // enable IR0 0-31
#define NVIC_ISER1 (*(volatile uint32_t*)0xE000E104UL) // enable IRQ 32-63

//NVIC interrupt priority registers - 1 byte per IRQ, 0=highest, 15=lowest
#define NVIC_IPR ((volatile uint8_t*)0xE000E400UL) // priority array

//NVIC interrupt clear-enable registers - each bit disables one IRQ
#define NVIC_ICER0 (*(volatile uint32_t*)0xE000E180UL) // disable IRQ 0-31
#define NVIC_ICER1 (*(volatile uint32_t*)0xE000E184UL) // disable IRQ 32-63

/*
    NVIC_enable
    Enables and IRQ and sets its priority
    irq: IRQ number (e.g. EXTI0_IRQn = 6, EXTI9_5_IRQn = 23)
    priority: 0 = highest, 15 = lowest
            Set lower priority (higher number) for non-criticla interrupts
            to avoid preempting time-sensitive ones like SysTick
    Ex:
        NVIC_enable(6, 1); // enable EXTI0, priority 1
*/
void NVIC_enable(uint8_t irq, uint8_t priority);

/*
    NVIC_disable
    Disables an IRQ by writing to the clear-enable register
    irq: IRQ number to disable
    Ex:
        NVIC_disable(6); //disable EXTI0
*/
void NVIC_disable(uint8_t irq);

#endif

