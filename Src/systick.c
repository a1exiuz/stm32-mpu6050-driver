#include "systick.h"
#include <stdint.h>

static volatile uint32_t ms_tick = 0;

void SysTick_Handler(void) {
    ms_tick++;
}

void SysTick_Init(void) {
    SYSTICK->LOAD = SYSTICK_LOAD_1MS; // reload value for 1ms
    SYSTICK->VAL = 0; // clear current value
    SYSTICK->CTRL = (1U << 2) | (1U << 1) | (1U << 0); 
    //             proccesosr clock enalbe - interrupt enable - enable SysTick

}

uint32_t get_tick(void) {
    return ms_tick;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_tick;
    while((ms_tick - start) < ms);
}

uint8_t elapsed(uint32_t start, uint32_t duration_ms) {
	return(get_tick() - start) >= duration_ms;
}
