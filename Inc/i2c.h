#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C1 ((I2C_RegMap_t*)0x40005400UL)

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
} I2C_RegMap_t;

void I2C1_init(void);

void I2C1_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);

uint8_t I2C1_read_reg(uint8_t dev_addr, uint8_t reg);

void I2C1_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif
