/**
* @file systick.h
* @brief SysTick timer driver for STM32F446RE
*
* Provides a 1ms timebase using the ARM Cortex-M4 SysTick timer.
* Exposes millisecond tick counter, blocking delay, and non-blocking
* elapsed time check utilities.
*
* @note SysTick is owned exclusively by this driver. 
*
* Base address: 0xE000E010
*
* @ref STM32F446RE Reference Manual RM0390 - Section 4 (SysTick)
* @ref ARM Cortex-M4 Generic User Guide - Section 4.4 (SysTick)
*/

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/** @brief SysTick peripheral base address */
#define SYSTICK ((SYSTICK_RegMap_t*)0xE000E010UL)

/** @brief SysTick reload value for 1ms tick at 16MHz: (16000000 / 1000) - 1 */
#define SYSTICK_LOAD_1MS (16000U - 1U)

/** @brief Debounce window for button inputs in milliseconds */
#define DEBOUNCE_MS 50U

/**
* @brief SysTick peripheral register map.
*
* Mapped directly to hardware at base address 0xE000E010.
* Each register is 32 bits wide.
*/
typedef struct {
    volatile uint32_t CTRL;  /**< 0x00 - Control and status (enable, interrupt, clock source) */
    volatile uint32_t LOAD;  /**< 0x04 - Reload value — counter resets to this on underflow */
    volatile uint32_t VAL;   /**< 0x08 - Current counter value — write any value to clear */
    volatile uint32_t CALIB; /**< 0x0C - Calibration value (TENMS, SKEW, NOREF flags) */
} SYSTICK_RegMap_t;

/**
* @brief SysTick interrupt handler 
*
* Called every 1ms by the SysTick timer. Increments the global
* millisecond counter used by get_tick(), delay_ms(), and elapsed()
*
* @note Do not call directly — invoked automatically by hardware on underflow.
*
* @retval  None
*/
void SysTick_Handler(void);

/**
* @brief Initializes SysTick for a 1ms interrupt period at 16MHz.
*
* Configures the reload value, clears the current value register
* and enables SysTick with processor clock and interrupt enabled
*
*
* @note Must be called before any use of get_tick(), delay_ms(), or elapsed().
*
* @retval None
*/
void SysTick_Init(void);

/**
* @brief Returns the current millisecond tick count
*
* Wraps around after ~49.7 days (UINT32_MAX ms).
* Use elapsed() for time comparisons to handle wraparound correctly.
*
* @retval uint32_t  Milliseconds elapsed since SysTick_Init() was called
*/
uint32_t get_tick(void);

/**
* @brief Blocking delay for a specified number of milliseconds.
*
* @param ms Number of milliseconds to delay
*
* @note Blocking — CPU is occupied for the full duration.
*       Do not call from an ISR.
*       Use elapsed() for non-blocking timing in the main loop.
*
* @retval None
*/
void delay_ms(uint32_t ms);

/**
* @brief Non-blocking check if a time duration has elapsed.
*
* Compares the current tick against a stored start time
*
* @param start Tick value captured at the start of the interval
* @param duration_ms  Duration to check in milliseconds
*
* Example:
* @code
*   uint32_t timer = get_tick();
*   if(elapsed(timer, 200)) {
*       timer = get_tick();
*       // do something every 200ms
*   }
* @endcode
*
* @retval 1 if duration_ms has elapsed since start
* @retval 0 if duration_ms has not yet elapsed
*/
uint8_t elapsed(uint32_t start, uint32_t duration_ms);

#endif /* SYSTICK_H */
