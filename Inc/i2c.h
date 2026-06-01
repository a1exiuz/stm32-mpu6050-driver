/**
* @file i2c.h
* @brief I2C1 driver for STM32F446RE
*
* Supports single byte register write, single byte register read, 
* and multi-byte burst read over I2C1 in standard at 100kHz
*
* Pin mapping:
*    - SCL: PB8 (AF4)
*    - SDA: PB9 (AF4)
*
* @note GPIO pins PB8 and PB9 must be configure as AF4 open drain via
*       GPIO_init before calling I2C1_init().
*
* Base address: 0x40005400
*
* @ref STM32F446RE Reference Manual RM0390 - Section 24 (I2C)
*/

#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/** @brief I2C1 peripheral base address */
#define I2C1 ((I2C_RegMap_t*)0x40005400UL)

/** 
* @brief I2C peripheral register amp
*
* Mapped directly to hardware at base address 0x40005400
* Each register is 32 bits wide
*/
typedef struct {
    volatile uint32_t CR1;   /**< 0x00 - Control 1: PE, START, STOP, ACK, SWRST */
    volatile uint32_t CR2;   /**< 0x04 - Control 2: FREQ, interrupt enable, DMA */
    volatile uint32_t OAR1;  /**< 0x08 - Own address 1 (slave mode, not used) */
    volatile uint32_t OAR2;  /**< 0x0C - Own address 2 (dual address, not used) */
    volatile uint32_t DR;    /**< 0x10 - Data register: write = TX, read = RX */
    volatile uint32_t SR1;   /**< 0x14 - Status 1: SB, ADDR, BTF, TXE, RXNE flags */
    volatile uint32_t SR2;   /**< 0x18 - Status 2: BUSY, MSL, TRA — read after SR1 */
    volatile uint32_t CCR;   /**< 0x1C - Clock control: I2C speed, fast/standard mode */
    volatile uint32_t TRISE; /**< 0x20 - Maximum rise time (set per I2C spec) */
    volatile uint32_t FLTR;  /**< 0x24 - Digital noise filter and analog filter */
} I2C_RegMap_t;

/** 
* @brief Initializes I2C1 at 100kHz standard mode with 16MHz PCLK1
*
* Configures CR2, CCR, and TRISE registers then enables I2C1
* 
* @note GPIO pins PB8 and PB9 must be configure as AF4 open drain via 
*      GPIO_init before calling this function.  
*
* @retval None
*/
void I2C1_init(void);

/**
* @brief Writes a single byte to a register on an I2C device
*
* Perfomrs a stnadard I2C write transaction:
* START -> address + write -> register address -> data byte -> STOP
*
* @param dev_adddr 7-bit I2C device address
* @param reg target register address on the device
* @param data Byte to write
*
* @note Blocking - spins until each bus condition is met
*       will hang indefinitely if the bus is stuck
*
* Example:
* @code
*   I2C1_write_reg(0x68, 0x6B, 0x00); // wake MPU6050
* @endcode
*
* @retval None
*/
void I2C1_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);

/**
* @brief Reads a single byte from a register on an I2C device
*
* Perfomrs a write-then-read transaction with a repeated START
* START -> address + write -> register -> repeated START -> address + write -> STOP -> data
*
* @param dev_addr 7-bit I2C device address
* @param reg Register address to read from
*
* @note Blocking - spins until each bus condition is met.
*       STOP is generated before reading DR per STM32 master receiver sequence
*
* Example:
* @code
*   uint8_t id = I2C1_read_reg(0x68, 0x75); // read WHO_AM_I
* @endcode
*
* @retval uint8_t Byte read from the specified register
*/
uint8_t I2C1_read_reg(uint8_t dev_addr, uint8_t reg);

/**
* @brief Reads multiple consecutive bytes form an I2C device using burst read
*
* Performs a write-then-read transaction with a repeated START
* START -> address + write -> register -> repeated START ->address + write -> data[0...n-1]
*
* @param dev_addr 7-bit I2C device address
* @param reg Starting register address 
* @param buf Pointer to buffer to store read data
* @param len Number of bytes to read
*
* @note Blocking - spins until each bus condition is met 
*       buf must be at least len bytes in size
*       
* Example:
* @code
*   uint8_t buf[6];
*   I2C1_read_burst(0x68, 0x38, buf, 6); // read accel X, Y, Z
* @endcode
*
* @retval None
*/
void I2C1_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif /* I2C_H */
