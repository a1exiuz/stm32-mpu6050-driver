#include "exti.h"
#include "nvic.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>

static volatile uint8_t button_flag1 = 0;
static volatile uint8_t button_flag2 = 0;
static volatile uint8_t button_flag3 = 0;

void EXTI_init(uint8_t pin, uint8_t port_code, uint8_t priority) {
    SET_BIT(RCC->APB2ENR, 14); // enable SYSFG clock

    uint8_t idx = pin / 4; // to see which EXTICR
    uint8_t shift = (pin % 4) * 4; // bit position within register

    SYSCFG->EXTICR[idx] &= ~(0xFU << shift);
    SYSCFG->EXTICR[idx] |= ((uint32_t)port_code << shift);

    EXTI->IMR |= (1U << pin); // unmask EXTI line

    //falling edge trigger (button active low: HIGH->LOW = press)
    EXTI->FTSR |= (1U << pin); // enable falling 
    EXTI->RTSR &= ~(1U << pin); //disable rising

    uint8_t irq;
    if(pin <= 4)
        irq = 6 + pin; // EXTI0-4 each have their own, starting at IRQ6
    else if (pin <= 9)
        irq = EXTI9_5_IRQn; // pins 5-9 share IRQ 23
    else 
        irq = EXTI15_10_IRQn; // pins 10-15 share IRQ 40

    NVIC_enable(irq, priority);
}

void EXTI0_IRQHandler(void) {
	 if(EXTI->PR & (1U << 0)) {
	        button_flag3 = 1;
	        EXTI->PR = (1U << 0);
	    }//depends what pin we are using rn pin 7 for button which uses EXTI 9_5_IRQ handler so well only do that one for now
}

void EXTI4_IRQHandler(void) {
    if(EXTI->PR & (1U << 4)) {
        button_flag2 = 1;
        EXTI->PR = (1U << 4);
    }
}

void EXTI9_5_IRQHandler(void) { // ISR - Interrupt service routing 
    if(EXTI->PR & (1U << 7)) { // read at that pin so for us rn pin 7
        button_flag1 = 1;
        EXTI->PR = (1U << 7); // clear - write 1 to clear
    }
}



uint8_t EXTI_get_flag1(void) {
    uint8_t flag1 = button_flag1;
    button_flag1 = 0; // clear when read
    return flag1;
}

uint8_t EXTI_get_flag2(void) {
    uint8_t flag2 = button_flag2;
    button_flag2 = 0;
    return flag2;
}

uint8_t EXTI_get_flag3(void) {
    uint8_t flag3 = button_flag3;
    button_flag3 = 0;
    return flag3;
}
