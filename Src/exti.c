/**
*@file exti.c
*@brief External Interrupt (EXTI) driver for STM32F446RE
*
*Confiuures GPIO pins as external interrupt sources with falling edge detection.
*Supports pins 0-15 across all GPIO ports via SYSCFG EXTICR mapping.
*IRQ routing: EXTI0-4 have dedicated handlers, EXTI5-9 and EXTI10-15 each share handlers 
*
*Three button inputs are supported via flag-based polling:
*   - Pin 0 (EXTI0)    : actively used, see EXTI0_IRQHandler
*   - Pin 4 (EXTI4)    : available for future use (e.g. reset, confirm)
*   - Pin 7 (EXTI9_5)  : available for future use (e.g. pattern change, back)
* To use pin 4 or 7, wire a button to the pin and poll EXTI_get_flag2()
* or EXTI_get_flag1() in your main loop.
*@ref STM32F446RE Reference Manual RM0390, Section 12 (EXTI)
*/
#include "exti.h"
#include "nvic.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>

/** @brief Internal flag set by EXTI9_5_IRQHandler, cleared on read via EXTI_get_flag1() */
static volatile uint8_t button_flag1 = 0;

/** @brief Internal flag set by EXTI15_10_IRQHandler, cleared on read via EXTI_get_flag2() */
static volatile uint8_t button_flag2 = 0;

/** @brief Internal flag set by EXTI0_IRQHandler, cleared on read via EXTI_get_flag3() */
static volatile uint8_t button_flag3 = 0;

/**
* @brief Configures an EXTI line for a given GPIO pin with falling edge detection
*
* @param pin GPIO_pin number (0-15)
* @param port_code SYSCFG port code (0=GPIOA, 1=GPIOB, 2=GPIOC, etc.)
* @param priority NVIC interrupt priority (0=highest, 15=lowest)
*
* @note Falling edge is used because the button is active-low (HIGH -> LOW on press).
*
* @retval None
*/
void EXTI_init(uint8_t pin, uint8_t port_code, uint8_t priority) {
    SET_BIT(RCC->APB2ENR, 14); /* enable SYSCFG clock */

    uint8_t idx = pin / 4; /* EXTI registers map 4 pins each, so determine which EXTICR to use */
    uint8_t shift = (pin % 4) * 4; /* bit position within register */

    /* Map GPIO port to EXTI line via SYSCFG_EXTICR */
    SYSCFG->EXTICR[idx] &= ~(0xFU << shift);
    SYSCFG->EXTICR[idx] |= ((uint32_t)port_code << shift);

    EXTI->IMR |= (1U << pin); /* un mask EXTI line */
    EXTI->FTSR |= (1U << pin); /* enable falling edge trigger */
    EXTI->RTSR &= ~(1U << pin); /* disable rising edge trigger */

    /* Determine IRQ number based on pin range */
    uint8_t irq;
    if(pin <= 4)
        irq = 6 + pin; /* EXTI0-4 each have their own, starting at IRQ6 */
    else if (pin <= 9)
        irq = EXTI9_5_IRQn; /* EXTI5-9 share IRQ 23 */
    else 
        irq = EXTI15_10_IRQn; /* EXTI10-15 share IRQ 40 */

    NVIC_enable(irq, priority);
}

/**
* @brief EXTI line 0 interrupt handler
*
* Handdles external interrupt on PA0 (EXTI0) - (connect button to PA0)
* Sets button_flag3
*
* @note Clear the PR flag insdie the handler by writign 1 to EXTI->PR
* @retval None
*/
void EXTI0_IRQHandler(void) {
	 if(EXTI->PR & (1U << 0)) {
	        button_flag3 = 1;
	        EXTI->PR = (1U << 0); /* clear pending flag by writing 1 */
	    }
}

/** 
* @brief EXTI line 4 interrupt handler
*
* Available for future use - not currrently connected
* EXTI_get_flag2() in the main loop to use
*
* @retval None
*/
void EXTI4_IRQHandler(void) {
    if(EXTI->PR & (1U << 4)) {
        button_flag2 = 1;
        EXTI->PR = (1U << 4);
    }
}

/**
 * @brief EXTI line 9-5 shared interrupt handler
 *
 * Available for future use - not currently connected
 * EXTI_get_flag1() in the main loop to use
 *
 * @retval None
 */
void EXTI9_5_IRQHandler(void) { /* ISR - Interrupt service routing */
    if(EXTI->PR & (1U << 7)) { /* read at that pin so for us rn pin 7 */
        button_flag1 = 1;
        EXTI->PR = (1U << 7); /* clear - write 1 to clear */
    }
}


/**
* @brief Returns and clears button flag 1 (pin 7, EXTI9_5)
*
* Available for future use - not currently connected
* Can use in other handlers not specifically EXTI9_5 if desired, just check PR for the correct pin
*
* @retval 1 if interrupt fired since last read
* @retval 0 otherwise
*/
uint8_t EXTI_get_flag1(void) {
    uint8_t flag1 = button_flag1;
    button_flag1 = 0; /* clear when read */
    return flag1;
}

/**
 * @brief Returns and clears button flag 2 (pin 4, EXTI4)
 *
 * Available for future use - not currently connected
 * Can use in other handlers not specifically EXTI4 if desired, just check PR for the correct pin
 *
 * @retval 1 if interrupt fired since last read
 * @retval 0 otherwise
 */
uint8_t EXTI_get_flag2(void) {
    uint8_t flag2 = button_flag2;
    button_flag2 = 0;
    return flag2;
}

/**
* @brief Returns and clears button flag 3 (pin 0, EXTI0)
* @retval 1 if interrupt fired since last read, 0 otherwise
 */
uint8_t EXTI_get_flag3(void) {
    uint8_t flag3 = button_flag3;
    button_flag3 = 0;
    return flag3;
}
