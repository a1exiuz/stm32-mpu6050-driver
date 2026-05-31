/**
*@file systick.c
*@brief SysTick timer driver for STM32F446RE
*
*
* Configures the Cortex-M4 SysTick timer to generate a 1ms interrupt
* providing a millisecond timebase for delyas and elapsed time checks
*
* Exposes three timing utilities:
*   - get_tick(): returns current millisecond count since startup
*   - delay_ms(ms): blocking delay in milliseconds
*   - elapsed(start, duration): non-blocking check if duration has passed
*
*@note SysTick is used exclusively by this driver
*
*@ref ARM Cortex-M4 Technical Reference Manual - Section 4 (SysTick)
*/

#include "systick.h"
#include <stdint.h>
/** @brief Millisecond counter incremented by SysTick_Handler every 1ms */
static volatile uint32_t ms_tick = 0;

/**
*@brief SysTick interrupt handler
*
* Called every 1ms by the SysTick timer. Increments the global
* millisecond counter used by get_tick(), delay_ms(), and elapsed()
*
*@retval None
*/
void SysTick_Handler(void) {
    ms_tick++;
}

/**
*@brief Initializes SysTick for a 1ms interrupt period
*
* Configures the reload value, clears the current value register
* and enables SysTick with processor clock and interrupt enabled
*
*@note Assumes system clock is 16MHz (HSI default)
*      Update SYSTICK_LOAD_1MS in systick.h if clock frequency changes
*
*@retval None
*/
void SysTick_Init(void) {
    SYSTICK->LOAD = SYSTICK_LOAD_1MS; /* reload value for 1ms */
    SYSTICK->VAL = 0; /* clear current value */
    SYSTICK->CTRL = (1U << 2) | (1U << 1) | (1U << 0); 
    /* processor clock enable | interrupt enable | SysTick enable */

}

/**
*@brief Returns the current millisecond tick count
*
* Tick count starts at 0 on startup and increments every 1ms
* Wraps around after ~49 days (UINT_MAX ms)
*
*@retval Current tick count in milliseconds since startup
*/
uint32_t get_tick(void) {
    return ms_tick;
}

/**
*@brief Blocking delay for a specified number of milliseconds
*
* Spins until the requested number of milliseconds have elapsed
* Uses subtraction-based comparison to hand uint32_t rollover correctly
*
*@param ms Number of milliseconds to delay
*
*@note Blocking - CPU is occupied for the full duration
*                 Use elapsed() for non-blocking timing in the main loop
*
*@retval None
*/
void delay_ms(uint32_t ms) {
    uint32_t start = ms_tick;
    while((ms_tick - start) < ms);
}

/**
*@brief Non-blocking check if a time duration has elapsed
*
* Compares the current tick against a stored start time
* Handles uint32_t rollover correctly via subrtraction 
*
*@param start Tick value captured at the start of the event
*@param duration_ms Duration to check against in milliseconds 
*
*@retval 1 if duration_ms has elapsed since start
*@retval 0 if duration_ms has not yet elapsed
*/
uint8_t elapsed(uint32_t start, uint32_t duration_ms) {
	return(get_tick() - start) >= duration_ms;
}
