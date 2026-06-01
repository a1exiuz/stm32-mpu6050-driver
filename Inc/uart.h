/**
* @file  uart.h
*
* @brief UART2 driver for STM32F446RE
*
* Provides blocking UART transmit over USART2 at 115200 8N1.
* Configured for debug output over the ST-Link virtual COM port.
*
* Pin mapping (fixed by Nucleo hardware, wired to ST-Link):
*   - TX : PA2 (AF7)
*   - RX : PA3 (AF7)
*
* Base address: 0x40004400
*
* @ref STM32F446RE Reference Manual RM0390 - Section 30 (USART)
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>

/** @brief USART2 peripheral base address */
#define USART2 ((USART_RegMap_t*)0x40004400UL)

/**
* @brief USART peripheral register map.
*
* Mapped directly to hardware at base address 0x40004400.
* Each register is 32 bits wide.
*/
typedef struct {
    volatile uint32_t SR;   /**< 0x00 - Status register (TXE, TC, RXNE, ORE flags)      */
    volatile uint32_t DR;   /**< 0x04 - Data register (write = TX, read = RX)            */
    volatile uint32_t BRR;  /**< 0x08 - Baud rate register (mantissa and fraction)       */
    volatile uint32_t CR1;  /**< 0x0C - Control 1 (UE, TE, RE, word length)              */
    volatile uint32_t CR2;  /**< 0x10 - Control 2 (stop bits, clock)                     */
    volatile uint32_t CR3;  /**< 0x14 - Control 3 (DMA, flow control, smartcard)         */
    volatile uint32_t GTPR; /**< 0x18 - Guard time and prescaler (smartcard, IrDA)        */
} USART_RegMap_t;

/**
* @brief Initializes USART2 at 115200 baud, 8N1 on PA2 (TX) and PA3 (RX).
*
* Enables GPIOA and USART2 clocks, configures PA2/PA3 as AF7,
* sets baud rate registers, and enables transmitter, receiver, and USART.
*
* @note Must be called before any UART2 transmit functions.
*
* @retval None
*/
void UART2_init(void);

/**
* @brief Transmits a single character over UART2
*
* Blocks until the transmit data register is empty 
* then writes the character.
*
* @param c Character to transmit
*
* @retval None
*/
void UART2_send_char(char c);

/**
* @brief Transmits a null-terminated string over UART2.
*
* @param s Pointer to null-terminated string to transmit
*
* @retval None
*/
void UART2_send_str(const char *s);

/**
* @brief Transmits a signed 32-bit integer as ASCII digits over UART2.
*
* Handles negative values by transmitting a '-' prefix.
*
* @param val Signed integer to transmit
*
* @retval None
*/
void UART2_print_int(int32_t val);

#endif /* UART_H */
