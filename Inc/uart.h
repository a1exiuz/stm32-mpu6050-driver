/*
    uart.h
    USART2 driver for STM32F446RE
    Debug output over ST-Link virtual COM port
    Tx: PA2, RX: PA3 (fixed by Nucleo hardware, wired to ST-Link)
    Baud: 115200, 8N1, no flow control
    Base address: 0x40004400
    Reference: STM32F446RE Reference Manual RM0390, Section 30
*/
#ifndef UART_H
#define UART_H

#include <stdint.h>

#define USART2 ((USART_RegMap_t*)0x40004400UL)

/*
    USART register map
    Each register is 32 bits wide
*/
typedef struct {
    volatile uint32_t SR;       //0x00 - status register (TXE, TC, RXNE, ORE flags)
    volatile uint32_t DR;       //0x04 - data register (write to transmit, read to receive)
    volatile uint32_t BRR;      //0x08 - baud rate register (mantissa and fraction)
    volatile uint32_t CR1;      //0x0C - control 1 (UE, TE, RE, word length)
    volatile uint32_t CR2;      //0x10 - control 2 (stop bits, clock)
    volatile uint32_t CR3;      //0x14 - control 3 (DMA, flow control, smartcard)
    volatile uint32_t GTPR;     //0x18 - guard tiame and prescaler (smartcard, IrDA)
} USART_RegMap_t;
 
/*
    UART2_init
    Initializes USART2 at 115200 baud, 8N1
    Enables TX and RX, configures PA2 and PA3 as AF7
    Must be called before any UART send functions
*/
void UART2_init(void);

/*
    UART2_send_char
    Transmits a single character over USART2
    Blocks until TX data register is empty (polling)
*/
void UART2_send_char(char c);

/*
    UART2_send_str
    Transmits a null-terminated string over USART2
    Calls UART2_send_char for character until null terminator
*/
void UART2_send_str(const char *s);

/*
    UART2_print_int
    Transmits a signed 32-bit integer as ASCII text over USART2
    Handles negative values by printing '-' prefix
*/
void UART2_print_int(int32_t val);

#endif
