#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#define SYSTICK ((SYSTICK_RegMap_t*)0xE000E010UL)
#define SYSTICK_LOAD_1MS (16000U - 1U) // 16MHz/ 1000- 1= 15999
#define DEBOUNCE_MS 50U

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SYSTICK_RegMap_t;

void SysTick_Handler(void);

void SysTick_Init(void);

uint32_t get_tick(void);

void delay_ms(uint32_t ms);

uint8_t elapsed(uint32_t start, uint32_t duration_ms);
#endif
