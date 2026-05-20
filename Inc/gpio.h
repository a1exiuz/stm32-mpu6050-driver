#ifndef GPIO_H
#define GPIO_H

#include "RCC.H"
#include <stdint.h>

#define GPIOA ((GPIO_RegMap_t*)0x40020000UL)
#define GPIOB ((GPIO_RegMap_t*)0x40020400UL)
#define GPIOC ((GPIO_RegMap_t*)0x40020800UL)

#define SET_BIT(reg, bit) ((reg) |= (1U << (bit)))
#define CLR_BIT(reg, bit) ((reg) &= ~(1U << (bit)))
#define TGL_BIT(reg, bit) ((reg) ^= (1U << (bit)))
#define READ_BIT(reg, bit) (((reg) >> (bit)) & 1U)

#define GPIO_SET(port, mask) ((port)->BSRR = (mask))
#define GPIO_CLR(port, mask) ((port)->BSRR = (mask) << 16U)

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_RegMap_t;

typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} GPIO_Mode_t;

typedef enum {
    GPIO_NOPULL,
    GPIO_PULLUP,
    GPIO_PULLDOWN
} GPIO_Pull_t;

typedef enum {
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIEUM,
    GPIO_SPEED_FAST,
    GPIO_SPEED_HIGH
} GPIO_Speed_t;

typedef enum {
    GPIO_LOW,
    GPIO_HIGH
} GPIO_State_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL,
    GPIO_OTYPE_OPENDRAIN
} GPIO_OType_t;

typedef struct {
    GPIO_RegMap_t *port;
    uint8_t clock_bit;
    uint32_t pin_mask;
    uint32_t pin_num;

    GPIO_Mode_t mode;
    GPIO_Pull_t pull;
    GPIO_OType_t type;

    GPIO_Speed_t speed;
    uint8_t alternate_func;
    
} GPIO_Config_t;

void GPIO_init(const GPIO_Config_t *cfg);

void GPIO_write(GPIO_Config_t *cfg, GPIO_State_t state);

void GPIO_toggle(GPIO_Config_t *cfg);

uint8_t GPIO_read(GPIO_Config_t *cfg);

#endif
