#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#define RCC ((RCC_RegMap_t*)0x40023800UL)

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLL_CFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    volatile uint32_t RESERVED1[1];
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t RESERVED2[1];
    volatile uint32_t RESERVED3[1];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    volatile uint32_t RESERVED4[1];
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t RESERVED5[1];
    volatile uint32_t RESERVED6[1];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    volatile uint32_t AHB3LPENR;
    volatile uint32_t RESERVED7[1];
    volatile uint32_t RESERVED8[1];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t RESERVED9[1];
    volatile uint32_t RESERVED10[1];
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2_SCFGR;
    volatile uint32_t PLL_SAI_CFGR;
    volatile uint32_t DCKCFGR;
    volatile uint32_t CKGATENR;
    volatile uint32_t DCKCFGR2;
} RCC_RegMap_t;

#endif 

