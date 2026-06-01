/**
* @file uart.c
*
* @brief UART2 driver for STM32F446RE
*
* Provides blocking UART transmit over USART2 at 115200 8N1
* Configured on PA2 (TX) and PA3 (RX) using alternate function AF7
*
* Exposes three transmmit utilities:
*   - UART2_send_char(): transmit a single character
*   - UART2_send_str(): transmit a null-terminated string
*   - UART2_print_int(): transmit a signed 32-bit integer as ASCII
*
* @note PA2 and PA3 must not be reconfigured elsewhere after calling
*      UART2_init() as this driver owns those pins
*
* @ref STM32F446RE Reference Manual RM0390, Section 30 (USART)
*/

#include "uart.h"
#include "gpio.h"

/**
* @brief Initializes USART2 at 115200 buad, 8N1 on PA2 (TX) and PA3 (RX)
*
* Enables GPIOA and USART2 clocks, configures PA2/PA3 as AF7
* sets baud rate, and enables transmitter, receiver, and USART
* 
* BRR calculation for 115200 buad at 16MHz:
* USARTDIV = 16000000 / (16 * 115200) = 8.68
* Mantissa = 8, Fraction = 0.68 * 16 = 10 (0xA)
*
* @retval None
*/
void UART2_init(void) {
    SET_BIT(RCC->AHB1ENR, 0); /* GPIOA clock enable */
    SET_BIT(RCC->APB1ENR, 17); /* USART2 clock enable */

    /* PA2, PA3 to alternate function mode (MODER = 10) */
    GPIOA->MODER &= ~((3U << 4) | (3U << 6)); 
    GPIOA->MODER |= ((2U << 4) | (2U << 6));

    /* PA2 = AFR[0] bits 11:8, PA3 = AFR[0] bits 15:12, both AF7 (USART2) */
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA->AFR[0] |= ((7U << 8) | (7U << 12));

    /* mantissa = 8, fraction = 10 -> 115200 baud*/
    USART2->BRR = (8U << 4) | (10U);

    USART2->CR1 = (1U << 3) | /* TE  TE transmit enable */
                  (1U << 2) | /* RE  RE receive enable */
                  (1U << 13); /* UE  UE USART enable */
}

/**
* @brief Transmits a single character over UART2
*
* Blocks until the transmit data register is empty 
* then writes the character
*
* @param c Character to transmit
*
* @retval None
*/
void UART2_send_char(char c) {
    while(!(USART2->SR & (1U << 7))); /* wait TXE */
    USART2->DR = (uint8_t)c;
}

/**
* @brief Transmits a null-terminated string over UART2
*
* Calls UART2_send_char() for each character until null terminator is reached
*
* @param s Pointer to null-termninated string to transmit
*
* @retval None
*/
void UART2_send_str(const char *s) {
    while(*s) {
        UART2_send_char(*s++);
    }
}

/**
* @brief Transmits a signed 32-bit integer as ASCII digits over UART2
*
* Handles negative values by transmitting a '-' prefix
* Digits are extracted in reverse order into a local buffer
* then transmitted in correct order
*
* @param val Signed integer to transmit
*
* @note Buffer is sized for 10 digits plus sign - sufficent for 
*      the full int32_t range (-2147483648 to 2147483647)
*
* @retval None
*/
void UART2_print_int(int32_t val) {
    char buf[12];
    int i = 0;

    if(val < 0) {
        UART2_send_char('-');
        val = -val;
    }

    if(val == 0) {
        UART2_send_char('0');
        return;
    }

    while(val > 0) {
        buf[i++] = '0' + (val % 10); /* extract digits least significant first */
        val /= 10;
    }

    
    for(int j = i - 1; j >= 0; j--) { /* print in reverse to get correct order */
        UART2_send_char(buf[j]);
    }
}
