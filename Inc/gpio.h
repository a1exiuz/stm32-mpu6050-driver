/*
    gpio.h
    General-purpose I/0s (GPIO) register map for STM32F446RE
    BASE ADDRESSES:
        GPIOA: 0x40020000 - GPIOB: 0x40020400 - GPIOC: 0x40020800
    Reference: STM32F446RE Reference Manual RM0390, Section 7
*/
#ifndef GPIO_H
#define GPIO_H

#include "RCC.H"
#include <stdint.h>

#define GPIOA ((GPIO_RegMap_t*)0x40020000UL)
#define GPIOB ((GPIO_RegMap_t*)0x40020400UL)
#define GPIOC ((GPIO_RegMap_t*)0x40020800UL)

// bit manupulation macros - operate on raw registers
#define SET_BIT(reg, bit) ((reg) |= (1U << (bit)))
#define CLR_BIT(reg, bit) ((reg) &= ~(1U << (bit)))
#define TGL_BIT(reg, bit) ((reg) ^= (1U << (bit)))
#define READ_BIT(reg, bit) (((reg) >> (bit)) & 1U)

// GPIO set/clear using BSRR - atomic, no read-modify-write needed
#define GPIO_SET(port, mask) ((port)->BSRR = (mask))
#define GPIO_CLR(port, mask) ((port)->BSRR = (mask) << 16U)

// Each register is 32 bits wide
typedef struct {
    volatile uint32_t MODER;        //0x00 - pin mode (input, output, AF, analog)
    volatile uint32_t OTYPER;       //0x04 - output type (push-pull or open-drain)
    volatile uint32_t OSPEEDR;      //0x08 - output spped (low, medium, fast, high)
    volatile uint32_t PUPDR;        //0x0C - pull-up/pull-down resistor config
    volatile uint32_t IDR;          //0x10 - input data register (read only)
    volatile uint32_t ODR;          //0x14 - output data register
    volatile uint32_t BSRR;         //0x18 - bit set/reset (atomic set/clear of ODR)
    volatile uint32_t LCKR;         //0x1C - lock register (locks pin config)
    volatile uint32_t AFR[2];       //0x20-0x24 - alternate function (AFR[0]=pins 0-7), AFR[1]=pins 8-15)
} GPIO_RegMap_t;

// pin mode - 2 bits per pin in MODER register
typedef enum {
    GPIO_MODE_INPUT = 0, // high impedance input
    GPIO_MODE_OUTPUT,    // general purpose output
    GPIO_MODE_AF,        // alternate function (I2C, UART, SPI etc)
    GPIO_MODE_ANALOG     // analog mode (ADC/DAC)
} GPIO_Mode_t;

// pull resistor - 2 bits per pin in PUPDR register
typedef enum {
    GPIO_NOPULL,         // floating input
    GPIO_PULLUP,         // internal pull-up to VDD
    GPIO_PULLDOWN        // internal pull-down to GND
} GPIO_Pull_t;

// output speed - 2 bits per pin in OSPEEDR register
typedef enum {
    GPIO_SPEED_LOW,     //2MHz
    GPIO_SPEED_MEDIEUM, //25MHz
    GPIO_SPEED_FAST,    //50MHz
    GPIO_SPEED_HIGH     //100MHz - use for I2C, SPI, UART
} GPIO_Speed_t;

// pin state for GPIO_write
typedef enum {
    GPIO_LOW,
    GPIO_HIGH
} GPIO_State_t;

// output type - 1 bit per pin in OTYPER register
typedef enum {
    GPIO_OTYPE_PUSHPULL, // drives high and low - default for most pins
    GPIO_OTYPE_OPENDRAIN // only drives low, needs external pull up - required for I2C
} GPIO_OType_t;

/*
    GPIO_Config_t
    Configuration struct passed to GPIO_init
    Fill all fields before calling GPIO_init
*/
typedef struct {
    GPIO_RegMap_t *port;    //GPIO port (GPIOA, GPIOB, GPIOC)
    uint8_t clock_bit;      //RCC AHBB1ENR for this port (0=A, 1=B, 2=C...)
    uint32_t pin_mask;      //bitmask of pin (1U << pin_num)
    uint32_t pin_num;       //pin number 0-15

    GPIO_Mode_t mode;       // input,output, AF, or analog
    GPIO_Pull_t pull;       // pull-up, pull-down, or none
    GPIO_OType_t type;      // push-pull or open drain

    GPIO_Speed_t speed;     // output speed
    uint8_t alternate_func; // AF number 0-15(only used when mode = GPIO_MODE_AF
} GPIO_Config_t;

/*
    GPIO_init
    Initializes a GPIO pin based on the provided config struct
    Enables port clock, sets mode, pull, type, speed, and AF if needed
    Must be called befdore any other GPIO Function on that pin
*/
void GPIO_init(const GPIO_Config_t *cfg);

/*
    GPIO_write
    Sets a pin HIGH or LOW using BSRR register (atomic operation)
    state: GPIO_HIGH or GPIO_LOW
*/
void GPIO_write(GPIO_Config_t *cfg, GPIO_State_t state);

/*
    GPIO_toggle
    Toggles the current state of an output pin
    Reads ODR to determine current stae then sets or clears via BSRR
*/
void GPIO_toggle(GPIO_Config_t *cfg);

/*
    GPIO_read
    Reads current state of a pin from IDR register
    Returns: 1 if pin is HIGH, 0 if LOW
*/
uint8_t GPIO_read(GPIO_Config_t *cfg);

#endif
