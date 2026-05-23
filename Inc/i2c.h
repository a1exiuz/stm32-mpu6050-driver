/*
    i2c.h
    I2C1 driver for STM32F446RE
    Supports single byte write ,single byte read, and multi-byte burst read
    SCL: PB8, SDA: PB9 (AF4)
    Speed: 100kHz standard mode, PCLK1 = 16MHz
    Base address: 0x40005400
    Reference: STM32F446RE Reference Manual RM0390, Section 24
*/
#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C1 ((I2C_RegMap_t*)0x40005400UL)

/*
    I2C register map
    Each register is 32 bits wide
*/
typedef struct {
    volatile uint32_t CR1;  //0x00 - control 1 (PE, START, STOP, ACK, SWRST)
    volatile uint32_t CR2;  //0x04 - control 2 (FREQ, interrupt enable, DMA)
    volatile uint32_t OAR1; //0x08 - own address 1 (slave mode, not used here)
    volatile uint32_t OAR2; //0x0C - own address 2 (dual address, not used here)
    volatile uint32_t DR;   //0x10 - data register (write to TX, read to RX)
    volatile uint32_t SR1;  //0x14 - status 1 (SB, ADDR, BTF, TXE, RXNE flags)
    volatile uint32_t SR2;  //0x18 - status 2 (BUSY, MSL, TRA flags) - must read after SR1
    volatile uint32_t CCR;  //0x1C - clock control (I2C speed, fast/standard mode)
    volatile uint32_t TRISE;//0x20 - maximum rise time (set per I2C spec)
    volatile uint32_t FLTR; //digital noise filter and analog filter
} I2C_RegMap_t;

/*
    I2C1_init
    initializes I2C1 peripheral for 100kHz standard mdoe
    Configures CR2, CCR, TRISE for 16MHz PCLK1
    GPIO pins PB8/PB9 must be configured as AF4 open drain via GPIO_cfg before calling
*/
void I2C1_init(void);

/*
    I2C1_write_reg
    Writes a single byte to a register on an I2C device
    Transaction: START -> addr+W -> reg -> data -> STOP
    dev_addr: 7-bit I2C device address
    reg: target register on the device
    data: byte to write
    Ex:
        I2C1_write_reg(0x68, 0x6B, 0x00); // wake MPU6050
*/
void I2C1_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);

/*
    I2C1_read_reg
    Reads a single byte from a register on an I2C device
    Transaction: START -> addr+W -> reg -> repeated START -> addr+R -> NACK -> STOP -> read
    dev_addr: 7-bit I2C device address
    reg: register address to read from
    returns: byte read from device
    Ex:
    I2C1_read_read(0x68, 0x75); // read WHO_AM_I
*/
uint8_t I2C1_read_reg(uint8_t dev_addr, uint8_t reg);

/*
    I2CI_read_burst
    Reads multiple consecutive bytes starting from a register address
    Transaction: START -> addr+W -> reg -> repeated START -> addr+R -> data[0..n-1] -> NACK -> STOP
    Uses ACK for all bytes execpt last (NACK signals end of read to slave)
    dev_addr: 7-bit I2C device address
    reg:      starting register address (slave auto-increments)
    buf:      pointer to buffer to store received bytes
    len:      number of bytes to read
    Ex:
        uint8_t buf[6];
        I2C1_read_burst(0x68, 0x3B, buf, 6); //read accel X, Y, Z
*/
void I2C1_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif
