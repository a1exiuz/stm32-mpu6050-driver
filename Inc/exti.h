/** 
* @file exti.h
*
* @brief External Interrupt (EXTI) driver for STM32F446RE
*
* Configures GPIO pins as external  interrupt sources with falling edge detection
* Supports pins 0-15 across all GPIO port via SYSCFG EXTICR mapping
*
*    Base addresses:
*       - EXTI: 0x40013C00
*       - SYSCFG: 0x40013800
* @ref STM32F446RE Reference Manual RM0390, Section 12 (EXTI)
*/

#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>

/** @brief EXTI peripheral base address */
#define EXTI ((EXTI_RegMap_t*)0x40013C00UL)

/** @brief SYSCFG peripheral base address */
#define SYSCFG ((SYSCFG_RegMap_t*)0x40013800UL)

/**
* @defgroup EXTI_IRQ EXTI IRQ Numbers
*
* @brief NVIC IRQ numbers for EXTI llines
*       Pins 0-4 have dedicated IRQs, pins 5-9 and 10-15 share IRQs.
*@{
*/
#define EXTI0_IRQn       6      /**< pin 0 - dedicated IRQ */
#define EXTI1_IRQn       7      /**< pin 1 - dedicated IRQ */
#define EXTI2_IRQn       8      /**< pin 2 - dedicated IRQ */
#define EXTI3_IRQn       9      /**< pin 3 - dedicated IRQ */
#define EXTI4_IRQn       10     /**< pin 4 - dedicated IRQ */
#define EXTI9_5_IRQn     23     /**< pins 5-9 - shared IRQ, check PR in handler */
#define EXTI15_10_IRQn   40     /**< pins 10-15 - shared IRQ, check PR in handler */
/** @} */

/** 
* @brief EXTI peripheral register map
*
* Controls interrupt masking, edge detection, and pending flags
* Mapped directly to hardware at base address 0x40013C00
*/
typedef struct {
    volatile uint32_t IMR;          /**< 0x00 - interrupt mask (1=unmasked/enabled) */
    volatile uint32_t RESERVED1[1]; /**< 0x04 - reserved (EMR even mask, not used) */
    volatile uint32_t RTSR;         /**< 0x08 - rising edge trigger select */
    volatile uint32_t FTSR;         /**< 0x0C - falling edge trigger select */
    volatile uint32_t RESERVED2[1]; /**< 0x10 - resereved (SWIER software interrupt) */
    volatile uint32_t PR;           /**< 0x14 - pending register (write 1 to clear) */
} EXTI_RegMap_t;

/** 
* @brief SYSCFG peripheral register map (partial)
*
* EXTICR registers map GPIO ports to EXTI lines
* Only EXTICR fields included - other SYSCFG registers not used
*/
typedef struct {
    volatile uint32_t RESERVED2[2]; /**< 0x00-0x04 - MEMRMP, PMC (not used) */
    volatile uint32_t EXTICR[4];    /**< 0x08-0x14 - EXTI port selection (4 bits per pin) */
    volatile uint32_t RESERVED3[2]; /**< 0x18-0x1C - reserved */
} SYSCFG_RegMap_t;

/**
* @brief Configures an EXTI line for a given GPIO pin with falling edge detection
*
* Enables SYSCFG clock, maps GPIO port to EXTI line, and enables via
* SYSCFG_EXTICRx, unmasks the interrupt line, configures falling edge
* detection, and enables the IRQ in the NVIC
*
* @param pin GPIO_pin number (0-15)
* @param port_code: SYSCFG port code (0=GPIOA, 1=GPIOB, 2=GPIOC, etc.)
* @param priority: NVIC interrupt priority (0=highest, 15=lowest)
*
* @note Falling edge is used because the button is active-low (HIGH -> LOW on press)
*
* Example:
* @code
*        EXTI_init(0, 0 , 1); // PA0, falling edge, pririoty 1
* @endcode
*
* @retval None
*/
void EXTI_init(uint8_t pin, uint8_t port_code, uint8_t priority);

/**
* @brief EXTI line 0 interrupt handler (pin 0)
*
* Handdles external interrupt on PA0 (EXTI0) - (connect button to PA0)
* Sets button_flag3
*
* @note Do not call directly - invoked automatically by hardware
*
* @retval None       
*/
void EXTI0_IRQHandler(void);  

/**
* @brief EXTI line 4 interrupt handler (pin 4)
*
* Available for future use. Wire a button to pin 4 and poll
* EXTI_get_flag2() in the main loop to use
*
* @note Do not call directly - invoked automatically by hardware
*
* @retval None
*/
void EXTI4_IRQHandler(void);   

/**
* @brief EXTI lines 9-5 shared interrupt handler
*
* Available for extension to other pins in
* this range by adding PR checks inside the handler 
* Currently handles pin 7
*
* @note Do not call directly - invoked automatically by hardware
*
* @retval None
*/
void EXTI9_5_IRQHandler(void);  

/**
* @brief Returns and clears button flag 1 (pin 7, EXTI9_5)
*
* Call once per main loop iteration. Flag  is set by EXTI9_5_IRQHandler
* and cleared automatically on read
*
* @note Not currently being used. Available for any EXTI_Handler
*
* @retval 1 if interrupt fired since last read
* @retval 0 otherwise 
*/
uint8_t EXTI_get_flag1(void);

/**
* @brief Returns and clears button flag 2 (pin 4, EXTI4)
*
* Call once per main loop iteration. Flag is set by EXTI4_IRQHandler
* and cleared automatically on read 
* 
* @note Not currently being used. Available for any EXTI_Handler
*
* @retval 1 if interrupt fired since last read
 *@retval 0 otherwise 
*/
uint8_t EXTI_get_flag2(void);

/** 
* @brief Returns and clears button flag 3 (pin 0, EXTI0)
*
* Call once per main loop iteration. Flag is set by EXTI0_IRQHandler
* and cleared automatically on read
*
* @retval 1 if interrupt fired since last read
* @retval 0 othervise 
*/
uint8_t EXTI_get_flag3(void);

#endif /* EXTI_H */
