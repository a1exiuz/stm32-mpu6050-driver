/*
    systick.h
    SysTick system timer driver for STM32F446RE
    Provides millisecond tick counter, delay, and elapsed time utilities
    Base address: 0xE000E010 (ARM Cortex-M4 core peripheral)
    Reference: STM32F446RE Reference Manual RM0390, Section 4
                ARM Cortex-M4 Generic User Guide, Section 4
*/
#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#define SYSTICK ((SYSTICK_RegMap_t*)0xE000E010UL)
#define SYSTICK_LOAD_1MS (16000U - 1U) // 16MHz, 1000- 1= 15999
#define DEBOUNCE_MS 50U                // 50ms debounce window for buttons

/*
    SysTick register map
    Each register is 32 bits wide
*/
typedef struct {
    volatile uint32_t CTRL; //0x00 - control and status (enable, interrupt, clock source)
    volatile uint32_t LOAD; //0x04 - reload value - counter resets to this value on underflow
    volatile uint32_t VAL;  //0x08 - current counter value - write any value to clear
    volatile uint32_t CALIB;//0x0C - calibration value (tenms field, SKEW, NOREF flags)
} SYSTICK_RegMap_t;

/*
    SysTick_Handler
    ISR - called automatically ever 1ms when SysTick underflows
    Increments ms_tick counter by get_tick, delay_ms, and elapsed
    Must not be called directly
*/
void SysTick_Handler(void);

/*
    SysTick_Init
    Initiaizes SysTick timer for 1ms interrupt at 16MHz
    Must be called before any use of delay_ms, get_tick, and elapsed
*/
void SysTick_Init(void);

/*
    get_tick
    Returns current millisecond tick count since SysTick_Init was called
    Wraps around after ~49 days (uint32_t overflow)
    Use elapsed() for time comparisons to handle wraparound correctly
*/
uint32_t get_tick(void);

/*
    delay_ms
    Blocking delay for specified number of milliseconds
    Uses get_tick internally - requires SysTick_Init to be called first
    Do not call from an ISR
*/
void delay_ms(uint32_t ms);

/*
    elapsed
    Returns 1 if duration_ms milliseconds have passed since start
    handles uint32_5 wraparound correctly
    Usage: if(elapsed(timer, 200)) { timer = get_tick(); ... }
*/
uint8_t elapsed(uint32_t start, uint32_t duration_ms);
#endif
