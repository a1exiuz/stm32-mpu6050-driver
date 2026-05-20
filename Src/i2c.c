#include "i2c.h"
#include "gpio.h"

void I2C1_init(void) {
    RCC->APB1ENR |= (1U << 21); // I2C1 clock

    I2C1->CR1 |=  (1U << 15);  // reset
    I2C1->CR1 &= ~(1U << 15);

    I2C1->CR2   = 16;  // 16MHz PCLK1
    I2C1->CCR   = 80;  // 100kHz
    I2C1->TRISE = 17;

    I2C1->CR1 |= (1U << 0); // enable
}

void I2C1_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data) {
    while(I2C1->SR2 & (1U << 1));       // wait not busy

    I2C1->CR1 |= (1U << 8);             // START
    while(!(I2C1->SR1 & (1U << 0)));    // wait SB

    I2C1->DR = (dev_addr << 1) & 0xFE; // address + write
    while(!(I2C1->SR1 & (1U << 1)));   // wait ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    // clear ADDR

    while(!(I2C1->SR1 & (1U << 7)));   // wait TXE
    I2C1->DR = reg;

    while(!(I2C1->SR1 & (1U << 7)));   // wait TXE
    I2C1->DR = data;

    while(!(I2C1->SR1 & (1U << 2)));   // wait BTF
    I2C1->CR1 |= (1U << 9);            // STOP
}

uint8_t I2C1_read_reg(uint8_t dev_addr, uint8_t reg) {
    while(I2C1->SR2 & (1U << 1));       // wait not busy

    I2C1->CR1 |= (1U << 8);             // START
    while(!(I2C1->SR1 & (1U << 0)));    // wait SB

    I2C1->DR = (dev_addr << 1) & 0xFE; // address + write
    while(!(I2C1->SR1 & (1U << 1)));   // wait ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    // clear ADDR

    while(!(I2C1->SR1 & (1U << 7)));   // wait TXE
    I2C1->DR = reg;

    while(!(I2C1->SR1 & (1U << 2)));   // wait BTF

    I2C1->CR1 |= (1U << 8);             // repeated START
    while(!(I2C1->SR1 & (1U << 0)));    // wait SB
//
    I2C1->DR = (dev_addr << 1) | 0x01; // address + read

    while(!(I2C1->SR1 & (1U << 1)));   // wait ADDR
    I2C1->CR1 &= ~(1U << 10);          // NACK after ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    // clear ADDR

    I2C1->CR1 |= (1U << 9);            // STOP

//
    while(!(I2C1->SR1 & (1U << 6)));   // wait RXNE

    I2C1->CR1 |= (1U << 10); 			// re-enalbe ack for next transaction
    return (uint8_t)I2C1->DR;
}

void I2C1_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
    if(len == 0) return;

    while(I2C1->SR2 & (1U << 1));       // wait not busy

    I2C1->CR1 |= (1U << 8);             // START
    while(!(I2C1->SR1 & (1U << 0)));    // wait SB

    I2C1->DR = (dev_addr << 1) & 0xFE; // address + write
    while(!(I2C1->SR1 & (1U << 1)));    // wait ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    // clear ADDR

    while(!(I2C1->SR1 & (1U << 7)));    // wait TXE
    I2C1->DR = reg;                     // send register address
    while(!(I2C1->SR1 & (1U << 2)));    // wait BTF

    I2C1->CR1 |= (1U << 8);             // repeated START
    while(!(I2C1->SR1 & (1U << 0)));    // wait SB

    I2C1->DR = (dev_addr << 1) | 0x01; // address + read
    I2C1->CR1 |= (1U << 10);            // ACK enable after address

    while(!(I2C1->SR1 & (1U << 1)));    // wait ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;                    // clear ADDR

    for(uint8_t i = 0; i < len; i++) {
        if(i == len - 2) {
            I2C1->CR1 &= ~(1U << 10);   // NACK on second to last
        }
        if(i == len - 1) {
            I2C1->CR1 |=  (1U << 9);    // STOP on last
        }
        while(!(I2C1->SR1 & (1U << 6))); // wait RXNE
        buf[i] = (uint8_t)I2C1->DR;
    }
}


