#include "nvic.h"
#include <stdint.h>

void NVIC_enable(uint8_t irq, uint8_t priority) {
    NVIC_IPR[irq] = priority << 4; // IPR array of bytes one per IRQ. Cortex - M4 only upper 4 bytes for priority
                                    // if you pass priority 1 = 0001 0000 
    if(irq < 32) //ISER is a bit field — each bit corresponds to one IRQ number. So to enable IRQ23 (EXTI9_5)
        NVIC_ISER0 |= (1U << irq);
    else // anything over 32= like irq40 (exti15_10)  - 1U << (40 - 32) = 1U << 8 → bit 8 in ISER1
        NVIC_ISER1 |= (1U << (irq - 32));
 }

void NVIC_disable(uint8_t irq) {
    if(irq < 32)
        NVIC_ICER0 |= (1U << irq); //Same logic as enable but uses ICER (Interrupt Clear Enable Register). 
    else                            //Writing a 1 to a bit in ICER disables that IRQ. Writing a 0 does nothing 
        NVIC_ICER1 |= (1U << (irq - 32)); // — so you can safely OR in the bit without affecting others.
}

