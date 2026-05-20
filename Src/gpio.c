#include "GPIO.H"
#include "RCC.H"
#include <stdint.h>
 
void GPIO_init(const GPIO_Config_t *cfg) {
    if(!cfg)
        return;

    SET_BIT(RCC->AHB1ENR, cfg->clock_bit);

    uint8_t pin = cfg->pin_num;
    cfg->port->MODER &= ~(3U << (pin * 2)); // bit width 0b1 == 1U , 0b11 == 3U , 0b111 == 7U, 0b1111 == 0xFU
    cfg->port->MODER |= (cfg->mode << (pin * 2));

    cfg->port->PUPDR &= ~(3U << (pin * 2));
    cfg->port-> PUPDR |= (cfg->pull << (pin * 2));

    if(cfg->mode == GPIO_MODE_AF) {
        uint8_t afr_idx = cfg->pin_num / 8; //Pin 0-7 -> /8 = 0 -> AFR[0] (AFRL) // Pin 8-15 -> /8 = 1 -> AFR[1] (AFRH)
        uint8_t afr_shift = (cfg->pin_num % 8) * 4; // % 8 so it resests back to 0-7 in whichever register ur in
                                                    // so (11 % 8) = 3 then 3*4 and 4 becasue 4 bit mask - 4bits wide
        cfg->port->AFR[afr_idx] &= ~(0xFU << afr_shift);
        cfg->port->AFR[afr_idx] |= ((uint32_t)cfg->alternate_func << afr_shift);
    }

    if(cfg->type == GPIO_OTYPE_OPENDRAIN) {
        cfg->port->OTYPER |= (1U << pin);
    } else {
        cfg->port->OTYPER &= ~(1U << pin);
    }

    cfg->port->OSPEEDR &= ~(3U << (pin * 2));
    cfg->port->OSPEEDR |= ((uint32_t)cfg->speed << (pin * 2));
}

void GPIO_write(GPIO_Config_t *cfg, GPIO_State_t state) {
    if(!cfg)
        return;
    
    if(state)
        GPIO_SET(cfg->port, cfg->pin_mask);
    else    
        GPIO_CLR(cfg->port, cfg->pin_mask);
}

void GPIO_toggle(GPIO_Config_t *cfg) {
    if(!cfg)
        return;
    
    if(cfg->port->ODR & cfg->pin_mask) // chccks currnet state of ur pin - by reading OUTPUT DATA Register if 1 then cleanr, if 0 then set
        GPIO_CLR(cfg->port, cfg->pin_mask);
    else 
        GPIO_SET(cfg->port, cfg->pin_mask);
}

uint8_t GPIO_read(GPIO_Config_t *cfg) {
    if(!cfg)
        return 0;

    return(cfg->port->IDR & cfg->pin_mask) ? 1U : 0U;
}
