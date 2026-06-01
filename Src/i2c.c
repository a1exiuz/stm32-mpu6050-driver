/**
* @file i2c.c
* @brief I2C1 driver for STM32F446RE
*
* Provides blocking I2C communication at 100kHz using the STM32F446RE
* I2C1 peripheral on PB8 (SCL) and PB9 (SDA).
* Supports single byte register write, single byte register read
* and multi-byte burst read over I2C1 in standard at 100kHz
*
* @note GPIO pins PB8 and PB9 must be configure as AF4 open drain via
*       GPIO_init before calling I2C1_init().
*      
*
* @ref STM32F446RE Reference Manual RM0390, Section 24 (I2C)
*/
#include "i2c.h"
#include "gpio.h"

/**
* @brief Initializes I2C1 peripheral at 100kHz standard mode with 16MHz PCKL1
*
* Resets the peripheral, configures clock speed, rise time, 
* and enables I2C1
*
* @note GPIO pins PB8 and PB9 must be configure as AF4 open drain via 
*       GPIO_init before calling this function.  
* @retval None
 */
void I2C1_init(void) {
    RCC->APB1ENR |= (1U << 21); /* enable I2C1 clock*/

    I2C1->CR1 |=  (1U << 15);  /* software reset*/
    I2C1->CR1 &= ~(1U << 15);

    I2C1->CR2   = 16;  /* PCLK1 = 16MHz */
    I2C1->CCR   = 80;  /* 100Khz standard mode: 16MHz / (2 * 100kHz) */
    I2C1->TRISE = 17;  /* max rise time: (1000ns / 62.5ns) + 1*/

    I2C1->CR1 |= (1U << 0); /* enable I2C1 */
}

/**
* @brief Writes a single byte to a register on an I2C device 
*
* Perfomrs a stnadard I2C write transaction:
* START -> address + write -> register address -> data byte -> STOP
*
* @param dev_addr 7-bit I2C device address
* @param reg target register address on the device
* @param data byte to write
*
* @note Blocking - spins until each bus condition is met
*      Will hang indefinitely if the bus is stuck
*
* @retval None
*/
void I2C1_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data) {
    while(I2C1->SR2 & (1U << 1)); /* wait until bus is not busy */  

    I2C1->CR1 |= (1U << 8);             /* START */
    while(!(I2C1->SR1 & (1U << 0)));    /* wait for start bit (SB) */

    I2C1->DR = (dev_addr << 1) & 0xFE; /* send address + write bit */
    while(!(I2C1->SR1 & (1U << 1)));   /* wait for ADDR flag*/
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    /* clear ADDR by reading SR1 then SR2*/

    while(!(I2C1->SR1 & (1U << 7)));   /* wait for TXE - data register empty */
    I2C1->DR = reg;

    while(!(I2C1->SR1 & (1U << 7)));   /* wait for TXE */
    I2C1->DR = data;

    while(!(I2C1->SR1 & (1U << 2)));   /* wait for BTF - byte transfer finished */
    I2C1->CR1 |= (1U << 9);            /* STOP */
}

/**
* @brief Reads a single byte from a register on an I2C device
*
* Perfomrs a write-then-read transaction with a repeated START
* START -> address + write -> register -> repeated START -> address + read -> STOP -> DATA
*
* @param dev_addr 7-bit I2C device address
* @param reg register address to read from
*
* @note Blocking - spins until each bus condition is met
*       STOP is generated before reading DR per STM32 master receiver sequence
*
*
* @retval uint8_t byte read from the specified register
*/
uint8_t I2C1_read_reg(uint8_t dev_addr, uint8_t reg) {
    while(I2C1->SR2 & (1U << 1));       /* wait until bus is not busy */

    I2C1->CR1 |= (1U << 8);             /* START */
    while(!(I2C1->SR1 & (1U << 0)));    /* wait for SB */

    I2C1->DR = (dev_addr << 1) & 0xFE; /* send address + write bit */
    while(!(I2C1->SR1 & (1U << 1)));   /* wait for ADDR flag */
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    /* clear ADDR */

    while(!(I2C1->SR1 & (1U << 7)));   /* wait for TXE */
    I2C1->DR = reg;                    /* send register address */

    while(!(I2C1->SR1 & (1U << 2)));   /* wait for BTF */

    I2C1->CR1 |= (1U << 8);             /* repeated START */
    while(!(I2C1->SR1 & (1U << 0)));    /* wait for SB */

    I2C1->DR = (dev_addr << 1) | 0x01; /* send address + read bit */
    while(!(I2C1->SR1 & (1U << 1)));   /* wait for ADDR flag */
    I2C1->CR1 &= ~(1U << 10);          /* NACK after ADDR */
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    /* clear ADDR */

    I2C1->CR1 |= (1U << 9);            /* STOP */

    while(!(I2C1->SR1 & (1U << 6)));   /* wait for RXNE - data ready */
    I2C1->CR1 |= (1U << 10); 		   /* re-enable ack for next transaction */
    
    return (uint8_t)I2C1->DR;
}

/**
* @brief Reads multiple consecutive bytes from an I2C device using burst read
*
* Perfroms a write-then-read transaction with a repeated START
* Uses ACK/NACK control to correclty terminate the read sequence
* NACK is sent on the second to last byte and STOP on the last byte
* per the STM32 I2C peripheral requirements for multi-byte reads
*
* @param dev_addr 7-bit I2C device address
* @param reg register address to read from
* @param buf pointer to buffer to store read data
* @param len number of bytes to read
*
* @note Blocking - spins until each bus condition is met
*      buf must be at least len bytes in size 
*      
* @retval None
*/
void I2C1_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
    if(len == 0) return;

    while(I2C1->SR2 & (1U << 1));       /* wait until bus is not busy */

    I2C1->CR1 |= (1U << 8);             /* START */
    while(!(I2C1->SR1 & (1U << 0)));    /* wait for SB */

    I2C1->DR = (dev_addr << 1) & 0xFE; /* send address + write bit */
    while(!(I2C1->SR1 & (1U << 1)));   /* wait for ADDR flag */
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    /* clear ADDR */

    while(!(I2C1->SR1 & (1U << 7)));   /* wait for TXE */
    I2C1->DR = reg;                    /* send register address */
    while(!(I2C1->SR1 & (1U << 2)));   /* wait for BTF */

    I2C1->CR1 |= (1U << 8);             /* repeated START */
    while(!(I2C1->SR1 & (1U << 0)));    /* wait for SB */

    I2C1->DR = (dev_addr << 1) | 0x01; /* send address + read bit */
    I2C1->CR1 |= (1U << 10);            /* ACK enable after address */

    while(!(I2C1->SR1 & (1U << 1)));   /* wait for ADDR flag */
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    /* clear ADDR */

    for(uint8_t i = 0; i < len; i++) {
        if(i == len - 2) {
            I2C1->CR1 &= ~(1U << 10);   /* NACK on second to last */
        }
        if(i == len - 1) {
            I2C1->CR1 |=  (1U << 9);    /* STOP on last */
        }
        while(!(I2C1->SR1 & (1U << 6))); /* wait for RXNE */
        buf[i] = (uint8_t)I2C1->DR;
    }
}


