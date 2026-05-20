#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

#define NVIC_ISER0 (*(volatile uint32_t*)0xE000E100UL) // enable IR0 0-31
#define NVIC_ISER1 (*(volatile uint32_t*)0xE000E104UL) // enable IRQ 32-63
#define NVIC_IPR ((volatile uint8_t*)0xE000E400UL) // priority array
#define NVIC_ICER0 (*(volatile uint32_t*)0xE000E180UL) // disable IRQ 0-31
#define NVIC_ICER1 (*(volatile uint32_t*)0xE000E184UL) // disable IRQ 32-63

void NVIC_enable(uint8_t irq, uint8_t priority);

void NVIC_disable(uint8_t irq);

#endif

