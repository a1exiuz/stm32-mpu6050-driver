/**
*@file main.c
*@brief MPU605 bare-metal sensor node for STM32 Nucleo-F446RE
*
* Initializes GPIO, I2C, UART, SysTick, EXTI, and MPU6050 drivers 
* Reads accelerometer and gyroscope data every 200ms and streams
* output over UART at 1115200 buad
*
* Button on PA0 cycles through 3 outpt modes:
*   - Mode 0: raw 16-bit signed ADC values
*   - Mode 1: scaled to g and degrees/s
*   - Mode 2: tilt direction based on accel data (LEFT/RIGHT/FORWARD/BACK/CENTER/FLAT/TILTED)
*
* LED on PA6 blinks at 1Hz as a heartbeat indicator
*
*@note No HAL or BSP
*
@ref STM32F446RE Reference Manual RM0390
*/
#include "systick.h"
#include "gpio.h"
#include "i2c.h"
#include "uart.h"
#include "mpu6050.h"
#include "exti.h"

#define FIRMWARE_VERSION "1.0.0"

static volatile uint8_t mode = 0;

/**
*@brief Application entry point
*
*Initializes all peripherals and enters the main polling loop
*
*@retval int Never returns in normal operation
*/
int main(void) {
    SysTick_Init();

    GPIO_Config_t led = {
        .port = GPIOA,
        .clock_bit = 0,
        .pin_num = 6,
        .pin_mask = (1U << 6),
        .mode = GPIO_MODE_OUTPUT,
        .type = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_NOPULL,
        .alternate_func = 0
    };

    GPIO_Config_t button = {
        .port = GPIOA,
        .clock_bit = 0,
        .pin_num = 0,
        .pin_mask = (1U << 0),
        .mode = GPIO_MODE_INPUT,
        .type = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_PULLUP,
        .alternate_func = 0
    };

    GPIO_Config_t scl = {
        .port = GPIOB,
        .clock_bit = 1,
        .pin_num = 8,
        .pin_mask = (1U << 8),
        .mode = GPIO_MODE_AF,
        .type = GPIO_OTYPE_OPENDRAIN,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULLUP,
        .alternate_func = 4
    };

    GPIO_Config_t sda = {
        .port = GPIOB,
        .clock_bit = 1,
        .pin_num = 9,
        .pin_mask = (1U << 9),
        .mode = GPIO_MODE_AF,
        .type = GPIO_OTYPE_OPENDRAIN,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULLUP,
        .alternate_func = 4
    };

    GPIO_Config_t tx = {
        .port = GPIOA,
        .clock_bit = 0,
        .pin_num = 2,
        .pin_mask = (1U << 2),
        .mode = GPIO_MODE_AF,
        .type = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NOPULL,
        .alternate_func = 7
    };

    GPIO_Config_t rx = {
        .port = GPIOA,
        .clock_bit = 0,
        .pin_num = 3,
        .pin_mask = (1U << 3),
        .mode = GPIO_MODE_AF,
        .type = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NOPULL,
        .alternate_func = 7
    };

    GPIO_init(&led);
    GPIO_init(&button);
    GPIO_init(&scl);
    GPIO_init(&sda);
    GPIO_init(&tx);
    GPIO_init(&rx);


    I2C1_init();
    UART2_init();
    EXTI_init(0, 0, 3);

    /* startup banner */
    UART2_send_str("====================\r\n");
    UART2_send_str("MPU6050 Sensor Node \r\n");
    UART2_send_str("Firmware: ");
    UART2_send_str(FIRMWARE_VERSION);
    UART2_send_str("\r\n");
    UART2_send_str("====================\r\n");

    /* who_am_i check*/
    uint8_t who = MPU6050_who_am_i();
    if(who == 0x68) {
        UART2_send_str("MPU6050 OK\r\n");
        MPU6050_init();
    } else {
        UART2_send_str("MPU6050 ERROR\r\n");
        while(1) {
            GPIO_toggle(&led);
            delay_ms(100);
        }
    }

    uint32_t uart_timer = get_tick();
    uint32_t led_timer  = get_tick();

    MPU6050_Data_t accel = {0};
    MPU6050_Data_t gyro  = {0};

    while(1) {
        /* button toggle */
        if(EXTI_get_flag3()) {
        	static uint32_t last_press = 0;
        	if((get_tick() - last_press) >= 200) {
        		last_press = get_tick();
        		EXTI->IMR &= ~(1U << 0);
        		mode = (mode + 1) % 3;
        		if(mode == 0) {
        			UART2_send_str("Mode: RAW\r\n");
        		}
        		else if(mode == 1) {
        		    UART2_send_str("Mode: CONVERTED\r\n");
        		}
        		else
        			UART2_send_str("Mode: DIRECTION\r\n");
        		EXTI->PR  =  (1U << 0);
        		EXTI->IMR |= (1U << 0);
        	}


        }

        /* read and print every 200ms */
        if(elapsed(uart_timer, 200)) {
            uart_timer = get_tick();

            accel = MPU6050_read_accel();
            gyro  = MPU6050_read_gyro();

            if(mode == 0) {
                MPU6050_print_raw(&accel, &gyro);
            } else if(mode == 1) {
                MPU6050_print_converted(&accel, &gyro);
            } else {
				 MPU6050_print_direction(&accel);
            }
        }

        /* blink LED every 1000ms */
        if(elapsed(led_timer, 1000)) {
            led_timer = get_tick();
            GPIO_toggle(&led);

        }
    }
}
