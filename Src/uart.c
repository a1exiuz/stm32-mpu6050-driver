#include "uart.h"
#include "gpio.h"

void UART2_init(void) {
    //enable GPIOA and USART2 clocks
    SET_BIT(RCC->AHB1ENR, 0);
    SET_BIT(RCC->APB1ENR, 17);

    //set PA2 and PA3 to alternate function mode (10)
    GPIOA->MODER &= ~((3U << 4) | (3U << 6));
    GPIOA->MODER |= ((2U << 4) | (2U << 6));

    //set AF7 (USART2) for PA2 and PA3
    //PA2 = AFR[0] bits 11:8, PA3 = AFR[0] bits 15:12
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA->AFR[0] |= ((7U << 8) | (7U << 12));

    //USARTDIV = 16000000 / (16 * 115200) = 8.68
    //Mantissa = 8
    //Fraction = 0.68 * 16 = 10 (0xA)
    USART2->BRR = (8U << 4) | (10U);

    //Enable TX, RX and USART
    USART2->CR1 = (1U << 3) | // TE transmit enable
                  (1U << 2) | // RE recieve enable
                  (1U << 13); // UE USART enable
}

void UART2_send_char(char c) {
    while(!(USART2->SR & (1U << 7))); // wait TXE
    USART2->DR = (uint8_t)c;
}

void UART2_send_str(const char *s) {
    while(*s) {
        UART2_send_char(*s++);
    }
}

void UART2_print_int(uint32_t val) {
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
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    // digits reversed, print backwards
    for(int j = i - 1; j >= 0; j--) {
        UART2_send_char(buf[j]);
    }
}
